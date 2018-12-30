#pragma once
#include <iostream>
using namespace std;

class BST
{
public:
	BST();
	~BST();
	struct BNode {//definition of the Nodes in a BST
				  //The nodes will be created in the heap section of our memory using the new function [malloc in C]
		int data;
		BNode* left;
		BNode* right;
	};
	BNode* getNewNode(int x) {	//this returns a new node with initial definition
		BNode* createNewNode = new BNode();
		createNewNode->data = x;	//(*createNewNode).data=x //another way for derefencing.
		createNewNode->left = createNewNode->right = NULL;	//setting left and right node to NULL
		return createNewNode;
	}
	BNode* Insert(BNode*, int);
	bool dataSearch(BNode*, int);
	void bstTestCase();
	void Print();
private:
	BNode* root;	//this stores address of root node
};
