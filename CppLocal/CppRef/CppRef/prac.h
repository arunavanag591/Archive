#pragma once
#ifndef PRAC_H
#define PRAC_H


#include <string> // for storing strings in a C++ way
#include <sstream> // to easily separate sentences into words
#include <vector> // to dynamically store arbitrary amounts of words
#include <algorithm> // for std::reverse
#include <iostream> // for printing the result

using namespace std;

class User {
private:
	int location;
	int maxsize;
	int count;
public:
	User();
	~User();
	void longestPalindrome(char[]);
	void reverseSentence(string);
	string longestPalindromeStr(string);
};

class Hash 
{
public:
	Hash();
	~Hash();
	int Hashed(string);
};

class Person {
public:
	Person();
	~Person();


};

class balanceParantheses {
public:
	balanceParantheses();
	~balanceParantheses();
	bool ArePair(char, char);
	bool AreParanthesesBalanced(string);
	void checkExample();
};
#endif // !PRAC_H