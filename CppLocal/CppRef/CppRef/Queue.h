#pragma once
#include <iostream>
using namespace std;

class Queue		//class containing Queue implementation using Linked List
{
public:
	Queue();
	~Queue();
	struct Node {
		int data;
		Node* next;
	};
	void Enqueue(int);
	void Dequeue();
	void Print();
	void testCase();
private:
	Node* front;
	Node* rear;
};

