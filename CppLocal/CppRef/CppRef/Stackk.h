#pragma once
#ifndef STACKK_H
#define STACKK_H

#include <iostream>
#define ARR_SIZE 101 

using namespace std;

class Stackk
{
public:
	Stackk();
	~Stackk();
	//array Implementation of a Stack
	void Push(int);
	void Pop();
	int Top();
	void Print();
	bool IsEmpty();
	void stackTestCase();
	
private:
	int A[ARR_SIZE];
	int top;
};

class LLStack 
{
public:
	LLStack();
	~LLStack();
	//Linked List implementation of a stack
	struct Node {
		int data;
		Node* next;
	};
	void Push(int);
	void Pop();
	void Top();
	void Print();
	bool IsEmpty();
	void llStackTestCase();
private:
	Node* head;
};

#endif