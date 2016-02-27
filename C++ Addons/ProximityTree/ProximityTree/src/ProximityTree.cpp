
#include "ProximityTree.h"

//Constructor 
ProximityTree::ProximityTree(int capacity, double ref_lat, double ref_long)
	: ref_lat(ref_lat), ref_long(ref_long)
{
	_array = new Node[capacity];
	this->capacity = capacity;
}

void ProximityTree::Insert(double latitude, double longitude)
{
	double global_dist = haversine(ref_lat, ref_long, latitude, longitude);

	//Find position for insertion
	
	//Check if the root is defined
	if (!_array[0].isInitialized()) {

	}

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
		node_T->parent = x;

	node_y.leftChild = T;
	if (node_T != nullptr)
		node_T->parent = y;

}

