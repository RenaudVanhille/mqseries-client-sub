#ifndef C_PROMISE_RUNNER_H
#define C_PROMISE_RUNNER_H

#include "CPromiseData.h"

#include <uv.h>

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Value;
using v8::Promise;
using v8::HandleScope;

namespace renaud
{
	template<class T>
	class CPromiseRunner
	{
	public:
		static void Do(const FunctionCallbackInfo<Value>& args)
		{
			Isolate* isolate = args.GetIsolate();

			CPromiseData* pData = new CPromiseData(isolate);
			if (!checkInputs(args, T::Def()))
				pData->Set("Wrong arguments", true);
			else
				pData->SetArgs(T::BuildArgs(args));

			uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));

			req->data = pData;
			uv_queue_work(uv_default_loop(), req, Run, Callback);

			Local<Promise::Resolver> local = Local<v8::Promise::Resolver>::New(isolate, pData->GetResolver());
			args.GetReturnValue().Set(local->GetPromise());
		}

	private:
		static void Run(uv_work_t* req)
		{
			CPromiseData* pData = static_cast<CPromiseData*>(req->data);
			if (pData->IsEmpty())
				T::Run(pData);
		}

		static void Callback(uv_work_t* req, int i)
		{
			Isolate* isolate = Isolate::GetCurrent();
			HandleScope scope(isolate);

			CPromiseData* pData = static_cast<CPromiseData*>(req->data);
			Local<Promise::Resolver> local = Local<Promise::Resolver>::New(isolate, pData->GetResolver());

			if (pData->IsOnError()) {
				local->Reject(String::NewFromUtf8(isolate, pData->GetResult().c_str()));
			}
			else {
				local->Resolve(String::NewFromUtf8(isolate, pData->GetResult().c_str()));
			}

			delete pData;
			isolate->RunMicrotasks();
		}
	};
}

#endif // C_PROMISE_RUNNER_H