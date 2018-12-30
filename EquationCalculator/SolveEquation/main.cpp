/*
File Name: main.cpp
Purpose: C++ main file for main operation
Author: Arunava Nag; Date Created: 09/25/2016
*/


#include "helper.h"	//header file for assisting main.cpp->contains all the directives and necessary includes
using namespace std;

/*
Function_Name: CheckAllValueOfMap	Type: Boolean
Purpose: To check if all RHS are solved. Returns true if all solved
*/
bool CheckAllValueOfMap(map<string, StoreMapValue> &myMap);

/*
Function_Name: Calculate	Type: Boolean
Purpose: Iterates over the map and solves equations until CheckAllValueOfMap returns true
*/
bool Calculate(map<string, StoreMapValue> &myMap);

/*
Function_Name: isInteger	Type:Boolean
Purpose: Checks if current string is an integer. Returns true if integer.
*/
bool isInt(const std::string & s);

/*
Function_Name: main
Purpose: main execution function
Operation Argumetns: Takes the .exe file | and file containing the equations
*/

int main(int argc, char** argv)
{
	//to ensure proper input
	if (argc != 2)
	{
		cout << "\nPlease enter valid arguments" << " ";
		return 0;
	}

	//Map of LHS as key and StoreMapValue Class as value
	map<string, StoreMapValue> EquationMap;

	string filePath = argv[1];
	ifstream file;
	file.open(filePath);
	if (!file.is_open())
	{
		cout << "\n File not opened";
		return 0;
	}
	string temp;
	string line;
	bool isKey = true;
	while (getline(file, line))
	{
		char * cstr = new char[line.length() + 1];
		strncpy(cstr, line.c_str(), line.length() + 1);

		char *pch = strtok(cstr, " =+");
		StoreMapValue mapvalue;
		string key;
		bool allNum = true;
		int valueofList = 0;
		while (pch != NULL)
		{
			//First string is LHS  and is stored as key
			if (isKey == true)
			{
				key = pch;
				isKey = false;
			}
			else
			{
				//List of strings are created to store in StoreMapValue Class

				bool isNum = isInt(pch);
				mapvalue.VariableList.push_back(pch);
				if (!isNum)
				{
					allNum = false;
				}
				else
				{
					valueofList = valueofList + stoi(pch);
				}
			}
			pch = strtok(NULL, " =+");
		}
		//allNum indicates all the variables are integers and RHS is solved
		mapvalue.setallValuesFound(allNum);
		//valueoflist is current value of all integers in list.
		mapvalue.setValue(valueofList);
		//Insert the value in sorted map for key
		EquationMap.insert({ key, mapvalue });

		isKey = true;

	}
	file.close();

	bool result = Calculate(EquationMap);

	//Print all the key value pairs
	map<string, StoreMapValue>::iterator it;
	for (it = EquationMap.begin(); it != EquationMap.end(); it++)
	{
		cout << it->first << " = " << it->second.getValue() << "\n";
	}

	return 0;
}