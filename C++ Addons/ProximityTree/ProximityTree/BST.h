
#include <stdint.h>

template <typename _Kty>
static bool LessThan(_Kty val1, _Kty val2) {
	return val1 <= val2;
}

template <typename _Kty>
static bool EqualTo(_Kty val1, _Kty val2) {
	return val1 == val2;
}

template <typename _Kty, typename _Vty>
class BST {

public:

	struct Node {
		_Kty key;
		_Vty value;
		Node* leftChild;
		Node* rightChild;
		Node* parent;
	};

	BST(uint64_t max_size, 
		bool(*lessThan)(_Kty, _Kty) = &LessThan, 
		bool(*equalTo)(_Kty, _Kty) = &EqualTo);

	const Node* Search(_Kty key);

	~BST();

private:

	const Node* _array = nullptr;
	Node* root = nullptr;
	uint64_t pos = 0u;

	bool(*lessThan)(_Kty, _Kty) = nullptr;
	bool(*equalTo)(_Kty, _Kty) = nullptr;

};


