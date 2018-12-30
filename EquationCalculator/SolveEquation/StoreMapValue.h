/*
File Name: StoreMapValue.h
Purpose: C++ Header file for class StoreMapValue.h
Author: Arunava Nag; Date Created: 09/25/2016
*/


#pragma once			//for newer support
#ifndef STOREMAPVALUE_H	//for legacy machines as well
#define STOREMAPVALUE_H

#include<string>
#include<list>

using namespace std;
class StoreMapValue
{
public:
	//Constructor
	StoreMapValue();
	//Destructor
	~StoreMapValue();

	//Function to get Values and set Values
	bool getallValuesFound();
	void setallValuesFound(bool valuesFound);
	int getValue();
	void setValue(int writeValue);

	//List to store RHS variables
	list<string> VariableList;

private:	//variables created in the private section to avoid mis handling by user
	int value; // Value of RHS
	bool allValuesFound; // bool if all variables are found
};

#endif	// STOREMAPVALUE_H