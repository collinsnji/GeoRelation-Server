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
		NODE_SET_PROTOTYPE_METHOD(tpl, "Insert", Insert);
		NODE_SET_PROTOTYPE_METHOD(tpl, "Remove", Remove);
		NODE_SET_PROTOTYPE_METHOD(tpl, "PrintOut", PrintOut);
		NODE_SET_PROTOTYPE_METHOD(tpl, "GetRoot", GetRoot);
		NODE_SET_PROTOTYPE_METHOD(tpl, "GetNearby", GetNearby);
		NODE_SET_PROTOTYPE_METHOD(tpl, "updateLocation", updateLocation);

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

	void ProximityTree_Wrap::Remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		//Gets the reference to the v8 engine
		Isolate* isolate = args.GetIsolate();

		if (args.Length() < 1) {
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Missing argument: NodeID")));
		}

		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());
		
		int32_t node = args[0]->IsUndefined() ? 0 : args[0]->Int32Value();
		obj->_tree.Remove(node);

	}

	void ProximityTree_Wrap::PrintOut(const v8::FunctionCallbackInfo<v8::Value>& args)
	{

		Isolate* isolate = args.GetIsolate();

		int32_t node = 0;
		if (args.Length() >= 1) {
			node = args[0]->IsUndefined() ? 0 : args[0]->Int32Value();
		}

		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());

		obj->_tree.PrintOut(node);

	}

	void ProximityTree_Wrap::GetRoot(const v8::FunctionCallbackInfo<v8::Value>& args)
	{

		Isolate* isolate = args.GetIsolate();

		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());

		uint32_t root = obj->_tree.getRoot(); 

		args.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, root));

	}

	void ProximityTree_Wrap::GetNearby(const v8::FunctionCallbackInfo<v8::Value>& args)
	{

		Isolate* isolate = args.GetIsolate();

		if (args.Length() < 2) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Missing arguments, required arguments for GetNearby() are NodeID and Benchmark")));
			return;
		}

		int32_t node_id = args[0]->Int32Value();
		double benchmark = args[1]->NumberValue();


		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());

		int arr_size;
		const ProximityTree::Nearby* result_array =  obj->_tree.FindNearby(node_id, benchmark, &arr_size);

		v8::Handle<v8::Array> arr = v8::Array::New(isolate, arr_size);

		for (int i = 0; i < arr_size; ++i) {
			v8::Local<v8::ObjectTemplate> nearby_templ = v8::ObjectTemplate::New(isolate);
			nearby_templ->SetInternalFieldCount(2);

			nearby_templ->Set(isolate, "Node", v8::Integer::New(isolate, result_array[i].node));
			nearby_templ->Set(isolate, "Distance", v8::Number::New(isolate, result_array[i].distance));
			
			arr->Set(i, nearby_templ->NewInstance());
		
		}

		args.GetReturnValue().Set(arr);

	}

	void ProximityTree_Wrap::updateLocation(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Isolate* isolate = args.GetIsolate();

		if (args.Length() < 3) {
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments for updateLocation")));
			return;
		}

		if (args[0]->IsUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Invalid rgument: NodeID")));
			return;
		}

		if (args[1]->IsUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Invalid Argument: latitude")));
			return;
		}

		if (args[2]->IsUndefined()) {
			isolate->ThrowException(v8::Exception::TypeError(
				String::NewFromUtf8(isolate, "Invalid Argument: Longitude")));
			return;
		}


		ProximityTree_Wrap* obj = ObjectWrap::Unwrap<ProximityTree_Wrap>(args.Holder());
		int32_t node = obj->_tree.update_node_location(args[0]->Int32Value(), args[0]->NumberValue(), args[1]->NumberValue());

		args.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, node));

	}

}  // namespace ProximityTree_Addon