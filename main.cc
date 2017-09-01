#include <iostream>

#include "Utils.h"

#include "Init.h"
#include "Close.h"
#include "On.h"

#include "Promise/CPromiseRunner.h"

using v8::Object;
using v8::Local;

namespace renaud
{
	void InitAll(Local<Object> exports) {
		NODE_SET_METHOD(exports, "init", CPromiseRunner<SInit>::Do);
		NODE_SET_METHOD(exports, "close", CPromiseRunner<SClose>::Do);
		NODE_SET_METHOD(exports, "on", CPromiseRunner<SOn>::Do);
	}

	NODE_MODULE(NodePlanif, InitAll)
}

/*
NODE_SET_METHOD(exports, "on", NodeOn);

// Events
void NodeOn(const FunctionCallbackInfo<Value>& args) {
if (!NodeCheckInputs(args, create_vector<std::string>("string")("func")))
return;

Isolate* isolate = args.GetIsolate();

String::Utf8Value utf8Event(args[0]);
Local<Function> callback = Local<Function>::Cast(args[1]);

Persistent<Function, CopyablePersistentTraits<Function>> saved;
saved.Reset(isolate, callback);
NodeEventFunctions[ToCString(utf8Event)].push_back(saved);

args.GetReturnValue().Set(Undefined(isolate));
}

// Liste des inscrits aux ?v?nements
typedef Persistent<Function, CopyablePersistentTraits<Function>> TSavedFunction;
typedef std::vector<TSavedFunction> TVecEventFunctions;
typedef std::map<std::string, TVecEventFunctions> TMapEventFunctions;
TMapEventFunctions NodeEventFunctions;

void OnProcessStateChanged(const char* szName, int iState) {
{ CLogBloc() << "Send event \"processStateChanged\"[" << szName << "," << iState << "] To Node" << log::NewLine(); }

SSendEvent2<std::string, int>* work = new SSendEvent2<std::string, int>("processStateChanged", szName, iState);
work->vecCallbacks = NodeEventFunctions["processStateChanged"];

work->request.data = work;
uv_queue_work(uv_default_loop(), &work->request, DoSendAsync, DoSendAsync2);
}

static void DoSendAsync(uv_work_t* req)
{

}

static void DoSendAsync2(uv_work_t* req, int status)
{
Isolate * isolate = Isolate::GetCurrent();
v8::HandleScope handleScope(isolate);

SSendEventWork* work = static_cast<SSendEventWork*>(req->data);
work->Do();
delete work;
}
*/