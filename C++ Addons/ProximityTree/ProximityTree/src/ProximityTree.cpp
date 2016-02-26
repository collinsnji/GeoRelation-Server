
#include "ProximityTree.h"

//Constructor 
ProximityTree::ProximityTree(int max_size, double ref_lat, double ref_long)
	: ref_lat(ref_lat), ref_long(ref_long)
{
	_array = new Node[max_size];
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

