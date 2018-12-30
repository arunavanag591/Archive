#include "Queue.h"
Queue::Queue()
{
	front = NULL;
	rear = NULL;
}

Queue::~Queue()
{
}

void Queue::Enqueue(int x) {	//Enqueue operation takes O(1) 
	Node* temp = new Node();
	temp->data = x;
	temp->next = NULL;

	if (front == NULL && rear == NULL) {
		front = rear = temp;
		return;
	}
	rear->next = temp;
	rear = temp;
	
}

void Queue::Dequeue() {	 // deleting from front takes O(1)
	Node* temp = front;
	if (front == NULL) return;
	if (front == rear) { front = rear = NULL; }
	else { front = front->next; }
	//anything in the dynamic memory needs to be explicitly free-ed and hence we use the free function
	//the temp still points to the old front which is deleted using the free function
	free(temp); 
	
}
void Queue::Print() {
	Node* temp = front;

	while(temp!=NULL) {
		cout << temp->data << endl;
		temp = temp->next;
	}
	cout << endl;
	delete temp;	//save memory
}
void Queue::testCase() {
	Queue* it = new Queue();
	it->Enqueue(2);
	it->Enqueue(3);
	it->Enqueue(4);
	it->Enqueue(21);
	it->Print();
	it->Dequeue();
	it->Dequeue();
	it->Print();
	delete it;	//save memory
}