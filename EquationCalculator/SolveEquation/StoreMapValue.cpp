/*
File Name: Map.cpp
Purpose: C++ file for class StoreMapValue
Author: Arunava Nag; Date Created: 09/25/2016
*/

#include"StoreMapValue.h"
#include<map>

using namespace std;
StoreMapValue::StoreMapValue()
{
	//constructor for default set values
	value = -1;
	allValuesFound = false;
}

StoreMapValue::~StoreMapValue()
{
	//default destructor for memory freeing operation
}

bool StoreMapValue::getallValuesFound()
{
	return allValuesFound;

}
void StoreMapValue::setallValuesFound(bool valuesFound)
{
	allValuesFound = valuesFound;
}

int StoreMapValue::getValue()
{
	return value;
}
void StoreMapValue::setValue(int writeValue)
{
	value = writeValue;
}

