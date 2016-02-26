
//Created by GabrielAlacchi on 2/26/2016 at 12:18:30pm
/*
	A binary search tree specialized for relating users locally by distance
*/

#include <stdint.h>

class ProximityTree {
public:
	struct Node {
		int32_t _nodeID = -1;
		double global_dist; //sorted by global_dist
		double latitude;
		double longitude;
		operator bool() { return _nodeID != -1; }
	};

	ProximityTree(int max_size);
	~ProximityTree();

private:
	Node* _array;
	uint32_t position = 0u;

};