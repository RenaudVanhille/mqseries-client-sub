#ifndef __ON_H__
#define __ON_H__

#include "Promise/CPromiseData.h"
#include "Sub/CSub.h"

#include "Utils.h"

#include <map>

#include <node/uv.h>

using v8::Function;
using v8::Handle;
using v8::Isolate;
using v8::String;
using v8::Persistent;
using v8::FunctionCallbackInfo;
using v8::CopyablePersistentTraits;

#include "Sub/ISubListener.h"

namespace renaud
{
	typedef Persistent<Function, CopyablePersistentTraits<Function>> TSavedFunction;
	typedef std::vector<TSavedFunction> TVecEventFunctions;
	typedef std::map<std::string, TVecEventFunctions> TMapEventFunctions;

	struct SSendData
	{
		TVecEventFunctions m_vecCB;
		std::string strMessage;
		uv_work_t req;
	};

	static void DoSendAsync(uv_work_t* req)
	{

	}

	static void DoSendAsync2(uv_work_t* req, int status)
	{
		Isolate * isolate = Isolate::GetCurrent();
		v8::HandleScope handleScope(isolate);

		SSendData* pData = (SSendData*)(req->data);
		Handle<Value> args[] = {
			String::NewFromUtf8(Isolate::GetCurrent(), pData->strMessage.c_str())
		};

		for (TVecEventFunctions::iterator it = pData->m_vecCB.begin(); it != pData->m_vecCB.end(); ++it)
			Local<Function>::New(isolate, (*it))->Call(isolate->GetCurrentContext()->Global(), 1, args);
		
		delete pData;
	}

	// Un seul listener qui dispach vers les intéressés
	class COnListeners : public ISubListener
	{
	private:
	public:
		COnListeners()
		{
			SUB.AddListener(this);
		}

		void Add(const std::string& rstrEvent, TSavedFunction func)
		{
			NodeEventFunctions[rstrEvent].push_back(func);
		}

	protected:
		virtual void OnMessage(const std::string& rstrEvent, const std::string& rstrMessage)
		{
			SSendData* pData = new SSendData();
			pData->strMessage = rstrMessage;
			pData->m_vecCB = NodeEventFunctions[rstrEvent];
			pData->req.data = pData;

			uv_queue_work(uv_default_loop(), &pData->req, DoSendAsync, DoSendAsync2);
		}

	private:
		TMapEventFunctions NodeEventFunctions;
	};

	// global = pas bien = mécréant
	COnListeners g_onListeners;

	struct SOn
	{
		struct SArgs
		{
			SArgs(const char* strEvent, Persistent<Function, CopyablePersistentTraits<Function>> func)
				: m_strEvent(strEvent),
				m_Func(func)
			{

			}

			std::string m_strEvent;
			Persistent<Function, CopyablePersistentTraits<Function>> m_Func;
		};

		static void* BuildArgs(const FunctionCallbackInfo<Value>& args)
		{
			Isolate* isolate = args.GetIsolate();

			String::Utf8Value arg1(args[0]);
			Local<Function> callback = Local<Function>::Cast(args[1]);
			Persistent<Function, CopyablePersistentTraits<Function>> saved;
			saved.Reset(isolate, callback);

			return new SArgs(*arg1, saved);
		}

		static TArgsDef Def() {
			return std::vector<std::string>({ "string", "func" });
		}

		static void Run(CPromiseData* pData)
		{
			const SArgs* pArgs = static_cast<const SArgs*>(pData->GetArgs());
			g_onListeners.Add(pArgs->m_strEvent, pArgs->m_Func);
			pData->Set("");
		}
	};
}

#endif // __ON_H__