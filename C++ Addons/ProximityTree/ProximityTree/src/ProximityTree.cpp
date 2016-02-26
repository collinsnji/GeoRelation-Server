
#include "ProximityTree.h"

//Constructor 
ProximityTree::ProximityTree(int max_size)
{
	_array = new Node[max_size];
}

ProximityTree::~ProximityTree()
{
	delete[] _array;
}

