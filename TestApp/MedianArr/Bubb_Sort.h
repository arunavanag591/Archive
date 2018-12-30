#pragma once
#include <iostream>
#define array_size(array) (sizeof((array))/sizeof((array[0])))


using namespace std;


class Bubb_Sort
{
public:
	Bubb_Sort();
	~Bubb_Sort();
	void bubbleSort(int arr[], int n);
	void checkBubbSort(int arr[], int n);
	void inputBubbSort();

	//pointer Experiment
	void expPoints();

	//check recursive function
	int findFactorial(int);
};

