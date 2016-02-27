
#include "ProximityTree.h"

//Constructor 
ProximityTree::ProximityTree(int capacity, double ref_lat, double ref_long)
	: ref_lat(ref_lat), ref_long(ref_long)
{
	_array = new Node[capacity];
	this->capacity = capacity;
}

ProximityTree::Node* ProximityTree::Insert(double latitude, double longitude)
{
	double global_dist = haversine(ref_lat, ref_long, latitude, longitude);
	
	uint32_t node_id = initNode(latitude, longitude, global_dist);
	return getNodeID(node_id);
}

uint32_t ProximityTree::RInsert(uint32_t index, double key)
{

}

ProximityTree::~ProximityTree()
{
	delete[] _array;
}

inline uint32_t ProximityTree::Height(uint32_t index)
{
	if (index < 0 || index >= capacity) return 0;
	Node& node = _array[index];
	return (node.isInitialized()) ? node.height : 0;
}

inline uint32_t ProximityTree::getBalance(uint32_t index)
{
	if (index < 0 || index >= capacity) return 0;
	Node& node = _array[index];
	return (node.isInitialized()) ?
		Height(node.leftChild) - Height(node.rightChild) : 0;
}

uint32_t ProximityTree::initNode(double lat, double lon, double dist)
{
	//TODO: Add functionality such that removed nodes get their places re used by new nodes

	Node& node = _array[position];
	node._nodeID = position++;
	node.global_dist = dist;
	node.latitude = lat;
	node.longitude = lon;
	return node._nodeID;
}

//AVL Tree leftRotate implementation
uint32_t ProximityTree::leftRotate(uint32_t x)
{
	Node& node_x = _array[x];
	uint32_t y = node_x.rightChild;

	Node& node_y = _array[y];
	Node* T = getNodeID(node_y.leftChild);

	//x takes ownership of y's left child
	node_x.rightChild = node_y.leftChild;
	node_y.parent = node_x.parent;

	//node_x gets rotated left
	node_y.leftChild = x;
	node_x.parent = y;

	if (T != nullptr) T->parent = x;

	Node* orig_parent = getNodeID(node_y.parent);
	if (orig_parent != nullptr) {
		if (orig_parent->leftChild == x)
			orig_parent->leftChild = y;
		else
			orig_parent->rightChild = y;
	}

	node_x.height = Max(Height(node_x.leftChild), Height(node_x.rightChild)) + 1;
	node_y.height = Max(Height(node_y.leftChild), Height(node_y.rightChild)) + 1;

	if (node_y.isRoot())
		root = node_y._nodeID;

	return y; 

}

uint32_t ProximityTree::rightRotate(uint32_t y)
{
	Node& node_y = _array[y];
	
	uint32_t x = node_y.leftChild;
	Node& node_x = _array[x];

	uint32_t T = node_x.rightChild;
	Node* node_T = getNodeID(T);

	node_x.parent = node_y.parent;
	node_y.leftChild = node_x.rightChild;

	node_x.rightChild = y;
	node_y.parent = x;

	if (node_T != nullptr) 
		node_T->parent = y;

	Node* orig_parent = getNodeID(node_x.parent);
	if (orig_parent != nullptr) {
		if (orig_parent->leftChild == y)
			orig_parent->leftChild = x;
		else
			orig_parent->rightChild = x;
	}

	node_x.height = Max(Height(node_x.leftChild), Height(node_x.rightChild)) + 1;
	node_y.height = Max(Height(node_y.leftChild), Height(node_y.rightChild)) + 1;

	if (node_x.isRoot())
		root = node_x._nodeID;

	return x;

}


