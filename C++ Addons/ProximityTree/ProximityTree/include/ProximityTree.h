
//Created by GabrielAlacchi on 2/26/2016 at 12:18:30pm
/*
	A binary tree specialized for relating users locally by distance
*/

#include <stdint.h>
#include "Haversine.h"
#include <stack>

namespace ProximityTree_Addon {

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
			bool isRoot() const { return parent == -1; }
		};

		ProximityTree(int capacity, double ref_lat, double ref_long);
		uint32_t Insert(double latitude, double longitude);
		void Remove(uint32_t node_id);

		Node* getNodeID(uint32_t id) {
			if (id >= capacity) return nullptr;
			Node* ptr = _array + id;
			return (ptr->_nodeID != -1) ? ptr : nullptr;
		}

		~ProximityTree();

	private:

		inline uint32_t Height(uint32_t index);
		inline uint32_t getBalance(uint32_t index);
		uint32_t initNode(double lat, double lon, double dist);
		inline void sever(Node& node, int32_t val = -1);
		inline void clearNode(uint32_t node_id);
		inline int32_t Sibling(Node& node);

		//AVL Tree leftRotate implementation
		uint32_t leftRotate(uint32_t x);
		uint32_t rightRotate(uint32_t y);

		uint32_t RInsert(uint32_t index, double dist);
		uint32_t RRemove(uint32_t index, double dist);

		uint32_t insert_index;

		uint32_t capacity;

		Node* _array;
		std::stack<uint32_t> Removed;

		const double ref_lat, ref_long;

		uint32_t position = 0u;
		uint32_t root = 0u;
		uint32_t size = 0u;

		int32_t minValueNode(uint32_t index);

	};

}