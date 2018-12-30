#include "Stackk.h"

Stackk::Stackk()
{//constructor for Array Implementation
	top = -1;
}
Stackk::~Stackk()
{
	//destructor for Array Implementation
}
LLStack::LLStack() 
{//constructor for Linked List Implementation
	head = NULL;
}
LLStack::~LLStack()
{
	//destructor for Linked List Implementation
}

void Stackk::Push(int x) {	// Push operation to insert an element on top of stack
	if (top == ARR_SIZE - 1) { cout << "Overflow" << endl; return; }

	A[++top] = x;	//top++; A[top] = x;

}
void Stackk::Pop() {	// Pop operation to remove an element from top of stack.
	if (top == -1) { cout << "Stack is already empty" << endl; return; }
	top--;
}
int Stackk::Top() {		// Top operation to return element at top of stack. 
	return A[top];
}

bool Stackk::IsEmpty() {	// This function will return 1 (true) if stack is empty, 0 (false) otherwise
	//if (top == -1) return 1;
	//return 0;
	bool result= top == -1 ? true : false;		//using ? operator and having a bool return type
	return result;
}

void Stackk::Print() {	//function to print and check the elements of the stack
	cout << "The stack elemensts are: ";
	for (int i = 0; i <= top; i++) {
			cout << A[i] << " ";
	}
	cout << endl;
}
void Stackk::stackTestCase() {	//function to check the above functions called in main
	Push(2); Print(); Push(1); Print(); Push(3); Print(); Push(4); Print();
	Pop(); Print();
}

/*Using Linked Lists Implementing Stack*/

void LLStack::Push(int x) {	//adding stack elements at the beginning of the list - O(1) time
	Node* temp = new Node();
	temp->data = x;
	temp->next = head;
	head = temp;
}

void LLStack::Pop() {	//function to remove elements from the head of the list
	Node* temp = head;
	if (head == NULL) return;
	head = head->next;
	//anything in the dynamic memory needs to be explicitly free-ed and hence we use the free function
	//the temp still points to the old head which is deleted using the free function
	free(temp);	
}

void LLStack::Top() {
	
}
void LLStack::Print() {
	Node* temp = head;
	cout << "The Stack elements are: " << " ";
	while (temp != NULL) {
		cout << temp->data << " ";
		temp = temp->next;
	}
	cout << endl;
	//delete temp;
}

bool LLStack::IsEmpty() {
	bool res= head == NULL ? true : false;
	return res;
}

void LLStack::llStackTestCase() {
	Push(2); Print(); Push(1); Print(); Push(3); Print(); Push(4); Print();
	Pop(); Print();
	
}