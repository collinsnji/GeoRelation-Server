
#include "ProximityTree.h"

using namespace ProximityTree_Addon;

//Constructor 
ProximityTree::ProximityTree(int capacity, double ref_lat, double ref_long)
	: ref_lat(ref_lat), ref_long(ref_long)
{
	_array = new Node[capacity];
	nearby_result_array = new Nearby[capacity];
	this->capacity = static_cast<uint32_t>(capacity);
}

uint32_t ProximityTree::Insert(double latitude, double longitude)
{
	double global_dist = haversine(ref_lat, ref_long, latitude, longitude);
	
	insert_index = initNode(latitude, longitude, global_dist);
	RInsert(root, global_dist); 

	++size;

	return insert_index;
}

uint32_t ProximityTree::RInsert(uint32_t index, double dist)
{
	//get a reference to the node
	if (index == -1 || size == 0) { 
		return insert_index;
	}

	Node& node = _array[index];

	if (dist < _array[index].global_dist)
		node.leftChild = RInsert(node.leftChild, dist);
	else
		node.rightChild = RInsert(node.rightChild, dist);

	node.height = Max(Height(node.leftChild), Height(node.rightChild)) + 1;

	int balance = getBalance(index);

	//Check the balance of the node 4 cases

	//LEFT LEFT
	if (balance > 1 && dist < _array[node.leftChild].global_dist) {
		return rightRotate(index);
	}

	//RIGHT RIGHT
	if (balance < -1 && dist < _array[node.rightChild].global_dist) {
		return leftRotate(index);
	}

	//Left Right
	if (balance > 1 && dist > _array[node.leftChild].global_dist) {
		node.leftChild = leftRotate(node.leftChild);
		return rightRotate(index);
	}

	//Right Left
	if (balance < -1 && dist < _array[node.rightChild].global_dist) {
		node.rightChild = rightRotate(node.rightChild);
		return leftRotate(index);
	}

	return index;

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

	if (Removed.size() > 0) {
		uint32_t pos = Removed.top();
		Removed.pop();
		Node& node = _array[pos];
		node._nodeID = pos;
		node.global_dist = dist;
		node.latitude = lat;
		node.longitude = lon;
		return pos;
	}
	else {
		Node& node = _array[position];
		node._nodeID = position++;
		node.global_dist = dist;
		node.latitude = lat;
		node.longitude = lon;
		return node._nodeID;
	}

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

uint32_t ProximityTree_Addon::ProximityTree::getRoot() const
{
	return this->root;
}

void ProximityTree_Addon::ProximityTree::Remove(uint32_t node_id)
{
	RRemove(root, _array[node_id].global_dist);
	--size;
	Removed.push(node_id);
}

const ProximityTree::Nearby* ProximityTree_Addon::ProximityTree::FindNearby(uint32_t node_id, double benchmark)
{
	_benchMark = benchmark;

	const Node& reference = _array[node_id];
	const double latitude = reference.latitude;
	const double longitude = reference.longitude;
	const double distance = reference.global_dist;
	
	//Find the highest node on the tree which fits within the distance benchmark
	int32_t index = node_id;

	while (haversine(latitude, longitude, _array[index].latitude, _array[index].longitude) < benchmark
		|| (index = _array[index].parent) != root);

	nearby_position = 0;

	NearbyTraversal(index, latitude, longitude, distance, 
		haversine(latitude, longitude, _array[index].latitude, _array[index].longitude));

	nearby_result_array[nearby_position].node = -1;

	return nearby_result_array;

}

void ProximityTree_Addon::ProximityTree::NearbyTraversal(int32_t index,
	const double lat, const double lon, const double ref_dist, const double dist_to_index)
{
	if (index < 0 || index >= capacity) return;

	Node& ref = _array[index];
	//Add the current node to the list
	Nearby& nearby = nearby_result_array[nearby_position++];
	nearby.distance = dist_to_index;
	nearby.node = index;
	
	Node& lRef = _array[ref.leftChild];
	Node& rRef = _array[ref.rightChild];

	double lDist = haversine(lRef.latitude, lRef.longitude, lat, lon);
	double rDist = haversine(rRef.latitude, rRef.longitude, lat, lon);

	if (lDist > _benchMark)
		NearbyTraversal(lRef._nodeID, lat, lon, ref_dist, lDist);
	if (rDist > _benchMark)
		NearbyTraversal(rRef._nodeID, lat, lon, ref_dist, rDist);

}


void ProximityTree_Addon::ProximityTree::PrintOut(int32_t index) const
{
	if (index < 0 || index >= static_cast<int32_t>(capacity)) return;
	Node& node = _array[index];
	if (node._nodeID == -1) return;
	fprintf_s(stdout, "Node %i\nLatitude: %f\nLongitude: %f \n\n", index, node.latitude, node.longitude);
	PrintOut(node.leftChild);
	PrintOut(node.rightChild);
}

int32_t ProximityTree_Addon::ProximityTree::update_node_location(uint32_t id, double lat, double lon)
{
	if (id >= capacity) return -1;
	Node& node = _array[id];

	double new_dist = haversine(lat, lon, ref_lat, ref_long);

	//First check parent
	bool reinsert = false;
	if (node.parent != -1) {
		Node& node_parent = _array[node.parent];
		reinsert = (node_parent.leftChild == node._nodeID && node_parent.global_dist <= new_dist)
			|| (node_parent.rightChild == node._nodeID && node_parent.global_dist > new_dist);
	}

	//Now check left child
	if (!reinsert && node.leftChild != -1) {
		Node& node_left = _array[node.leftChild];
		reinsert = (node_left.global_dist > new_dist);
	}

	if (!reinsert && node.rightChild != -1) {
		Node& node_right = _array[node.rightChild];
		reinsert = (node_right.global_dist < new_dist);
	}

	if (reinsert) 
	{
		Remove(id);
		return Insert(lat, lon);
	}
	else {
		node.global_dist = new_dist;
		node.latitude = lat;
		node.longitude = lon;
		return id;
	}

}

uint32_t ProximityTree_Addon::ProximityTree::RRemove(uint32_t index, double dist)
{

	if (index == -1 || !_array[index].isInitialized())
		return index;

	Node& node = _array[index];

	if (dist < node.global_dist)
		node.leftChild = RRemove(node.leftChild, dist);

	else if (dist > node.global_dist)
		node.rightChild = RRemove(node.rightChild, dist);

	else 
	{

		if (node.leftChild == -1 && node.rightChild == -1) 
		{
			sever(node);
			clearNode(index);
		}
		else if (node.leftChild != -1 && node.rightChild == -1) {
			sever(node, node.leftChild);
			clearNode(index);
		}
		else if (node.leftChild == -1 && node.rightChild != -1) {
			sever(node, node.rightChild);
			clearNode(index);
		}
		else {

			int32_t temp = minValueNode(node.rightChild);

			node.global_dist = _array[temp].global_dist;
			node.latitude = _array[temp].latitude;
			node.longitude = _array[temp].longitude;

			node.rightChild = RRemove(node.rightChild, _array[temp].global_dist);

		}

	}

	node.height = Max(Height(node.leftChild), Height(node.rightChild)) + 1;

	int balance = getBalance(index);
	int lBalance = getBalance(node.leftChild);
	int rBalance = getBalance(node.rightChild);

	if (balance > 1 && lBalance >= 0)
		return rightRotate(index);

	if (balance > 1 && lBalance < 0) {
		node.leftChild = leftRotate(node.leftChild);
		return rightRotate(index);
	}

	if (balance < -1 && rBalance <= 0)
		return leftRotate(index);

	if (balance < -1 && rBalance > 0) {
		node.rightChild = rightRotate(node.rightChild);
		return leftRotate(index);
	}

	return index;
}

int32_t ProximityTree_Addon::ProximityTree::minValueNode(uint32_t index)
{

	while (_array[index].leftChild != -1)
		index = _array[index].leftChild;

	return index;
}

inline void ProximityTree_Addon::ProximityTree::sever(Node& node, int32_t val /*= -1*/)
{
	if (node.parent != -1) {
		Node& parent = _array[node.parent];
		if (val != -1)
			_array[val].parent = parent._nodeID;
		if (parent.leftChild == node._nodeID)
			parent.leftChild = val;
		else
			parent.rightChild = val;
	}
	else if (val != -1)
		root = val;
	else
		root = 0;
}

inline void ProximityTree_Addon::ProximityTree::clearNode(uint32_t node_id)
{
	Node& node = _array[node_id];
	node._nodeID = -1;
	node.parent = -1;
	node.leftChild = -1;
	node.rightChild = -1;
}

inline int32_t ProximityTree_Addon::ProximityTree::Sibling(Node& node)
{
	if (node.parent != -1) {
		Node& parent = _array[node.parent];
		if (parent.leftChild == node._nodeID)
			return parent.rightChild;
		else
			return parent.leftChild;
	}
	return -1;
}

