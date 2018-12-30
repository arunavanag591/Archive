#include "LinkedLists.h"
using namespace std;

doubleLL::doubleLL() {	//constructor for doubly Linked List
	head = NULL;
}

doubleLL::~doubleLL() {	//destructor for doubly Linked List

}


void doubleLL::insertAtHead(int a) {
	node* newNode = getNewNode(a);
	if (head == NULL) {
		head = newNode;
		return;
	}
	head->prev = newNode;
	newNode->next = head;
	head = newNode;
}

void doubleLL::insertAtTail(int b) {
	node* tailNode = getNewNode(b);
	if (head == NULL) {
		head = tailNode;
		return;
	}
	else {
		node* temp = head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = tailNode;
	}
}

void doubleLL::printDL() {
	//forward printing
	node* temp = head;
	while (temp != NULL) {
		cout << temp->data;
		temp = temp->next;
	}
	cout << endl;
}

void doubleLL::reversePrint() {
	//reverse printing
	node* temp = head;
	if (temp== NULL) return;
	while (temp->next != NULL) {
		temp = temp->next;
	}
	//traversing backwards
	while (temp != NULL) {
		cout << temp->data;
		temp = temp->prev;
	}
	cout << endl;
}

void doubleLL::testCase() {
	insertAtHead(2); cout << endl; printDL(); cout << endl; reversePrint(); cout << endl;
	insertAtHead(4); cout << endl; printDL(); cout << endl; reversePrint(); cout << endl;
	insertAtHead(6); cout << endl; printDL(); cout << endl; reversePrint(); cout << endl;
	insertAtHead(5); cout << endl; printDL(); cout << endl; reversePrint(); cout << endl;
	/*insertAtTail(2);
	insertAtTail(3);
	insertAtTail(4);
	insertAtTail(5);*/
	//TODO: for insertAtTail reversePrint donot work
}