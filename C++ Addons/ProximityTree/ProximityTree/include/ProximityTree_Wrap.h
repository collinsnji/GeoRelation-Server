
// myobject.h
#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
#include <node_object_wrap.h>
#include "ProximityTree.h"

namespace ProximityTree_Addon {

	class ProximityTree_Wrap : public node::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);

	private:
		explicit ProximityTree_Wrap(int capacity, double ref_lat, double ref_long);
		~ProximityTree_Wrap();

		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Insert(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void Remove(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void PrintOut(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void GetRoot(const v8::FunctionCallbackInfo<v8::Value>& args);

		static v8::Persistent<v8::Function> constructor;
		ProximityTree _tree;
	};

}  // namespace ProximityTree_Addon

#endif