
//Created by GabrielAlacchi on 2/26/2016 at 12:18:30pm
/*
	A binary tree specialized for relating users locally by distance
*/

#include <stdint.h>
#include "Haversine.h"

class ProximityTree {
public:
	struct Node {
		int32_t _nodeID = -1;
		double global_dist; //sorted by global_dist
		double latitude;
		double longitude;
		Node* leftChild;
		Node* rightChild;
		Node* parent;
		bool isInitialized() const { return _nodeID != -1; }
	};

	ProximityTree(int max_size, double ref_lat, double ref_long);
	void Insert(double latitude, double longitude);
	void InsertAt(uint32_t position, Node* parent, bool leftChild) {

	}

	Node* getNodeID(uint32_t id) {
		Node* ptr = _array + id;
		return (ptr->_nodeID != -1) ? ptr : nullptr;
	}

	~ProximityTree();

private:
	Node* _array;
	const double ref_lat, ref_long;
	uint32_t position = 0u;

};