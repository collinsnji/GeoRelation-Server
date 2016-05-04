// addon.cc
#include <node.h>
#include "ProximityTree_Wrap.h"

namespace ProximityTree_Addon {

	using v8::Local;
	using v8::Object;

	void InitAll(Local<Object> exports) {
		ProximityTree_Wrap::Init(exports);
	}

	NODE_MODULE(ProximityTree, InitAll)

}  // namespace Addon