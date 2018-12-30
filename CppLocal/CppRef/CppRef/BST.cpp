#include "BST.h"
/*
NOTE:Comparison chart :
Time taken by a BST for for:
BST		|	array(unsorted)	        |	Linked Lists	        |	array(sorted) |
---------------------------------------------------------------------------------------------------------------
search(x): O(logn)	|	O(n)			|	O(n)			|	O(logn)	      |
insert(x): O(logn)	|	O(1)			|	O(1)			|	O(n)          |
remove(x): O(logn)	|	O(n)			|	O(n)			|	O(n)          |
---------------------------------------------------------------------------------------------------------------
which makes it much faster than
1. array (unsorted)
2. Linked Lists
3. array (sorted)
*/
BST::BST()
{
	root = NULL;
}
BST::~BST()
{
}

BST::BNode *BST::Insert(BNode* root, int data) {
	if (root == NULL) {
		root = getNewNode(data);		// a new node is created
	}
	else if (data <= root->data) {
		root->left = Insert(root->left, data);	// using a recursive call to insert in the left when smaller
	}
	else {
		root->right = Insert(root->right, data);
	}
	return root;
}

bool BST::dataSearch(BNode* root, int data) {
	if (root == NULL) return false;
	if (root->data == data)return true;
	else if (data <= root->data) return dataSearch(root->left, data);
	else return dataSearch(root->right, data);

}

void BST::bstTestCase() {
	root = Insert(root, 21);
	root = Insert(root, 2);
	root = Insert(root, 1);
	root = Insert(root, 20);
	root = Insert(root, 9);
	int x;
	cout << "Type the number that you want to search: ";
	cin >> x;
	if (dataSearch(root, x) == true) cout << "Number is Found" << endl;
	else cout << "Not Found" << endl;
}

void BST::Print() {
}