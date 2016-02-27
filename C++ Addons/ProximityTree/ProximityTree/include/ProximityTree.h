
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

		int16_t height = 0;

		int32_t leftChild = -1;
		int32_t rightChild = -1;
		int32_t parent = -1;
		bool isInitialized() const { return _nodeID != -1; }
	};

	ProximityTree(int capacity, double ref_lat, double ref_long);
	void Insert(double latitude, double longitude);

	Node* getNodeID(uint32_t id) {
		if (id < 0 || id >= capacity) return nullptr;
		Node* ptr = _array + id;
		return (ptr->_nodeID != -1) ? ptr : nullptr;
	}

	~ProximityTree();

private:

	inline uint32_t Height(uint32_t index);

	inline uint32_t getBalance(uint32_t index);

	//AVL Tree leftRotate implementation
	uint32_t leftRotate(uint32_t x);
	uint32_t rightRotate(uint32_t y);

	int capacity;

	Node* _array;
	const double ref_lat, ref_long;
	uint32_t position = 0u;

};