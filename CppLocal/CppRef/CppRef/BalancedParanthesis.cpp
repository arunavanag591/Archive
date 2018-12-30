/*
C++ Program to check for balanced parentheses in an expression using stack.
Given an expression as string comprising of opening and closing characters
of parentheses - (), curly braces - {} and square brackets - [], we need to
check whether symbols are balanced or not.
*/

#include "prac.h"
#include <iostream>
#include <stack>

using namespace std;

//Implementing stack for checking balanced parantheses
balanceParantheses::balanceParantheses() {}	
balanceParantheses::~balanceParantheses() {}

bool balanceParantheses::ArePair(char opening, char closing){

	if (opening == '(' && closing == ')') return true;
	else if (opening == '{' && closing == '}') return true;
	else if (opening == '[' && closing == ']') return true;
	return false;
}

bool balanceParantheses::AreParanthesesBalanced(string exp) {

	stack<char> checkParantheses;
	for (int i = 0; i < exp.length(); i++) {
		if (exp[i] == '(' || exp[i] == '{' || exp[i] == '[')
			checkParantheses.push(exp[i]);
		else if (exp[i] == ')' || exp[i] == '}' || exp[i] == ']' ) {
			if (checkParantheses.empty() || !ArePair(checkParantheses.top(), exp[i]))
				return false;
			else
				checkParantheses.pop();
		
		}
			
	}
	
	return checkParantheses.empty() ? true : false;
}

void balanceParantheses::checkExample() {
	string expression;
	cout << "Enter an expression:  "; // input expression from STDIN/Console
	cin >> expression;
	if (AreParanthesesBalanced(expression))
		cout << "Balanced\n";
	else
		cout << "Not Balanced\n";

}