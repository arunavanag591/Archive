#include "LinkedLists.h"
using namespace std;
Node::Node() {}
Node::~Node() {}

LinkedLists::LinkedLists()
{
	//default constructor
	top = NULL;
}


LinkedLists::~LinkedLists()
{
}



void LinkedLists::addNode(int val){	//function to add a node at the end of the linked list
	Node* temp = new Node(); //creating a new node
	temp->data = val;
	temp->next = NULL;
	if (top == NULL) {
		top = temp;	//checking the head or else feel with data(val)
	}
	else {
		Node* temp1 = top;
		while (temp1->next != NULL) {
			temp1 = temp1->next;
		}	
		temp1->next = temp;
	}

};

void LinkedLists::reverseList() {	//function to reverse a Linked List
	Node* current, *prev, *next;
	current = top;
	prev = NULL;	//initially points to NULL
	
	while (current != NULL) {
		next = current->next; 
		current->next = prev;
		prev = current;
		current = next;
	}
	top = prev;	//to change the top/head node to the last node
}

void LinkedLists::revRecur(Node* p) {//reverse linedList using recursion
	if (p->next == NULL) { //exit base case
		top = p;
		return;
	}
	revRecur(p->next); 
	Node* q = p->next;//p->next->next=q;
	q->next = p;	// |_q_|<-|_p_| ... reversing 
	p->next = NULL;//setting the last node to NULL
}

void LinkedLists::insert(int data, int n) { // function to insert a node at nth position
	Node* tempX = new Node();
	tempX->data = data;
	tempX->next = NULL;
	if (n == 1) {//inserting at the head position
		tempX->next = top;
		top = tempX;
		return;
	}
	Node* tempY = top;
	for (int i = 0; i < n - 2; i++) {
		tempY = tempY->next;	//tempY points to (n-1)th node
	}
	tempX->next = tempY->next;
	tempY->next = tempX;
}
Node *LinkedLists::InsertAtTail(Node* head, int data){//function that returns a node and inserts at the tail
	Node* tmp = new Node();
	tmp->data = data;
	tmp->next = NULL;
	if (head == NULL) {
		head = tmp;
		return head;
	}
	Node* current = head;
	while (current->next != NULL) {
		current = current->next;
	}
	current->next = tmp;
	return head;

}

void LinkedLists::deleteNode(int n) {
	Node* temp1 = top;
	if (n == 1) {//deleting the head node
		top = temp1->next;
		delete temp1;
		return;
	}
	for (int i = 0; i < n - 2; i++) {
		temp1 = temp1->next;
		//temp1 points to (n-1)th node
	}
	Node* temp2 = temp1->next; //nth node
	temp1->next = temp2->next;// (n+1)th node
	delete temp2;

}

void LinkedLists::printList() { //function to print the linked list
	Node* temp = top;
	while (temp != NULL) {
		cout << temp->data << endl;
		temp = temp->next;
	}
	cout << endl;
}

void LinkedLists::printRecur(Node* tempN) {	//recursion print uses more memory than iterative approach
	tempN = top; 
	if (tempN == NULL) return;//exit condition
	printRecur(tempN->next);	//recursion call
	cout << tempN->data << endl;//printing the value in the node
	
}

void LinkedLists::testList() {	//test function to check reverse operation of a linked list
	LinkedLists* ttl = new LinkedLists();
	ttl->addNode(20);
	ttl->addNode(21);
	ttl->addNode(22);
	ttl->addNode(23);
	ttl->addNode(24);
	ttl->addNode(25);
	cout << "The current list has the following items: " << endl;
	ttl->printList();
	ttl->reverseList();
	cout << "This is the reversed list items: " << endl;
	ttl->printList();
	delete ttl;			//freeing up the memory

}

void LinkedLists::insertTest() {
	//top = NULL;
	LinkedLists* itt = new LinkedLists();
	itt->insert(2, 1);
	itt->insert(3, 2);
	itt->insert(4, 1);
	itt->insert(5, 4);
	itt->printList();
	delete itt;
}

void LinkedLists::recurPrintTest() {
	Node* head = NULL;
	//TODO create a insert function for this test

}

void LinkedLists::deleteTest() {
	LinkedLists* dt = new LinkedLists();
	dt->addNode(22);
	dt->addNode(24);
	dt->addNode(21);
	dt->addNode(25);
	dt->printList();
	//user input->selecting the position/node that needs to be deleted
	int pos;
	cin >> pos;
	dt->deleteNode(pos);
	dt->printList();
	delete dt;
}
