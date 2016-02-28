// myobject.cc
#include "ProximityTree_Wrap.h"
#include <stdint.h>

namespace ProximityTree_Addon {

	using v8::Function;
	using v8::FunctionCallbackInfo;
	using v8::FunctionTemplate;
	using v8::Isolate;
	using v8::Local;
	using v8::Number;
	using v8::Object;
	using v8::Persistent;
	using v8::String;
	using v8::Value;

	Persistent<Function> ProximityTree_Wrap::constructor;

	ProximityTree_Wrap::ProximityTree_Wrap(int capacity, double ref_lat, double ref_long)
		: _tree(capacity, ref_lat, ref_long) 
	{

	}

	ProximityTree_Wrap::~ProximityTree_Wrap() {
	}

	void ProximityTree_Wrap::Init(Local<Object> exports) {
		Isolate* isolate = exports->GetIsolate();

		// Prepare constructor template
		Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
		tpl->SetClassName(String::NewFromUtf8(isolate, "ProximityTree"));
		tpl->InstanceTemplate()->SetInternalFieldCount(1);

		// Prototype
		NODE_SET_PROTOTYPE_METHOD(tpl, "plusOne", PlusOne);
		NODE_SET_PROTOTYPE_METHOD(tpl, "Insert", Insert);

		constructor.Reset(isolate, tpl->GetFunction());
		exports->Set(String::NewFromUtf8(isolate, "ProximityTree"),
			tpl->GetFunction());
	}

	//BINDING TO THE CONSTRUCTOR
	void ProximityTree_Wrap::New(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

		if (args.IsConstructCall()) {
			// Invoked as constructor: `new MyObject(...)`

			if (args.Length() < 3) {
				isolate->ThrowException(v8::Exception::TypeError(
					String::NewFromUtf8(isolate, "Wrong number of arguments for constructor of ProximityTree")
					));
			}

			int32_t capacity = args[0]->IsUndefined() ? 0 : args[0]->Int32Value();
			double ref_lat = args[1]->IsUndefined() ? 0 : args[1]->NumberValue();
			double ref_lon = args[2]->IsUndefined() ? 0 : args[2]->NumberValue();
			ProximityTree_Wrap* obj = new ProximityTree_Wrap(capacity, ref_lat, ref_lon);
			obj->Wrap(args.This());
			args.GetReturnValue().Set(args.This());
		}
		else {
			// Invoked as plain function `MyObject(...)`, turn into construct call.
			const int argc = 3;

			if (args.Length() < 3) {
				isolate->ThrowException(v8::Exception::TypeError(
					String::NewFromUtf8(isolate, "Wrong number of arguments for constructor of ProximityTree")
					));
			}

			Local<Value> argv[argc] = { args[0], args[1], args[2] };
			Local<Function> cons = Local<Function>::New(isolate, constructor);
			args.GetReturnValue().Set(cons->NewInstance(argc, argv));
		}
	}

	void ProximityTree_Wrap::PlusOne(const FunctionCallbackInfo<Value>& args) {
		Isolate* isolate = args.GetIsolate();

		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());
		obj->value_ += 1;

		args.GetReturnValue().Set(Number::New(isolate, obj->value_));
	}

	//args: latitude, longitude 
	void ProximityTree_Wrap::Insert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		//Gets the reference to the v8 engine
		Isolate* isolate = args.GetIsolate();
		
		if (args.Length() < 2) {
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments for Insert")));
		}

		if (args[0]->IsUndefined())
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Invalid argument: latitude")));

		if (args[1]->IsUndefined()) 
			isolate->ThrowException(v8::Exception::TypeError(
			String::NewFromUtf8(isolate, "Invalid argument: longitude")));
		

		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());
		uint32_t node = obj->_tree.Insert(args[0]->NumberValue(), args[1]->NumberValue());

		args.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, node));

	}

}  // namespace ProximityTree_Addon