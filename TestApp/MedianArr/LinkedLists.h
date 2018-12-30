#pragma once
#include <iostream>
using namespace std;

class Node {
public:
	Node();
	~Node();
	int data;
	Node* next;
};


class LinkedLists
{
public:
	LinkedLists();
	~LinkedLists();
	void addNode(int);
	void reverseList();
	void revRecur(Node*);
	void printList();
	void printRecur(Node*);
	void insert(int, int);
	Node* InsertAtTail(Node*, int);
	void deleteNode(int);
	void testList();
	void insertTest();
	void deleteTest();
	void recurPrintTest();
private:
	Node* top = NULL;

};

class doubleLL {
public:
	doubleLL();
	~doubleLL();
	struct node {
		int data;
		node* prev;
		node* next;
	};

	struct node* getNewNode(int x) {
		node* newNode=new node();//creating a node on stack
		newNode->data = x;
		newNode->prev = NULL;
		newNode->next = NULL;
		return newNode;
	}
	void insertAtHead(int);
	void insertAtTail(int);
	void testCase();

	void printDL();
	void reversePrint();

private:
	node* head = NULL;
};
