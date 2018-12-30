/*
File Name: helper.h
Purpose: C++ Header file for functions and include definitions to help the main operation or main.cpp
Author: Arunava Nag; Date Created: 09/25/2016
*/

#pragma once		//for newer support (not yet standardized)
#ifndef HELPER_H	//for legacy machines as well
#define HELPER_H

#pragma warning(disable:4996)
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "StoreMapValue.h"


bool CheckAllValueOfMap(map<string, StoreMapValue> &myMap)
{
	map<string, StoreMapValue>::iterator it;
	for (it = myMap.begin(); it != myMap.end(); it++)
	{
		if (it->second.getallValuesFound() == false)
			return false;
	}
	return true;
}

bool isInt(const std::string & s)
{
	if (s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false;

	char * p;
	strtol(s.c_str(), &p, 10);
	return (*p == 0);
}




bool Calculate(map<string, StoreMapValue> &myMap)
{
	map<string, StoreMapValue>::iterator it;
	while (!CheckAllValueOfMap(myMap))
	{
		for (it = myMap.begin(); it != myMap.end(); it++)
		{
			if (it->second.getallValuesFound() == false)
			{
				list<string>::iterator strit;
				bool listover = true;
				for (strit = it->second.VariableList.begin(); strit != it->second.VariableList.end(); strit++)
				{
					bool isCurrInt = isInt(*strit);
					if (!isCurrInt)
					{
						map<string, StoreMapValue>::iterator pos = myMap.find(*strit);
						if (pos != myMap.end())
						{
							if (pos->second.getallValuesFound())
							{
								*strit = to_string(pos->second.getValue());
							}
							else
							{
								listover = false;
								break;
							}
						}
					}
				}
				if (listover)
				{
					list<string>::iterator finalListIt;
					unsigned int finalVal = 0;
					for (finalListIt = it->second.VariableList.begin(); finalListIt != it->second.VariableList.end(); finalListIt++)
					{

						finalVal += stoi(*finalListIt);

					}
					it->second.setValue(finalVal);
					it->second.setallValuesFound(true);
				}
			}
		}
	}
	return true;
}	

#endif // HELPER_H