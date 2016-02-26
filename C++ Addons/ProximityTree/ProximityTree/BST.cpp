
#include "BST.h"

//This is the most abominable thing I've ever created

template <typename _Kty, typename _Vty>
BST<_Kty, _Vty>::BST(uint64_t max_size,
	bool(*lessThan)(_Kty, _Kty) = &LessThan,
	bool(*equalTo)(_Kty, _Kty) = &EqualTo)
	: _array(new Node[max_size])
{

	this->lessThan = lessThan;
	this->equalTo = equalTo;
}


template <typename _Kty, typename _Vty>
BST<_Kty, _Vty>::~BST()
{
	delete[] _array;
}

