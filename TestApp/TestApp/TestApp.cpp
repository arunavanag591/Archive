// TestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "merger.h"
#include <iostream>
#include<array>
#include<vector>

#define array_size(array) (sizeof((array))/sizeof((array[0])))

using namespace std;
template <typename T>
void merge_sort(vector<T>& arr, vector<T>& arr1, vector<T>& arr2) {
	arr.clear();
	int i = 0, j = 0, k = 0;

	for (i = 0; i < arr1.size() && j < arr2.size(); k++) {
		if (arr1.at(i) <= arr2.at(j)) {
			arr.push_back(arr1.at(i));
			i++;
		}
		else if (arr1.at(i) > arr2.at(j)) {
			arr.push_back(arr2.at(j));
			j++;
		}
		k++;
	}
	while (i < arr1.size()) {
		arr.push_back(arr1.at(i));
		i++;
	}

	while (j < arr2.size()) {
		arr.push_back(arr2.at(j));
		j++;
	}

};

template <typename T>
vector<T>merge(std::vector<T>& arr) {
	if (1 < arr.size()) {
		vector<T> arr1(arr.begin(), arr.begin() + arr.size() / 2);
		merge(arr1);//dividing to size 1

		std::vector<T> arr2(arr.begin() + arr.size() / 2, arr.end());
		merge(arr2);
		merge_sort(arr, arr1, arr2);

	}
	return (arr);
	//write_vector(arr);
};


int main()
{
	//Merge Sort
	
	vector<int> inputVec;
	int size = 6;
	cout << "Input Elements" << endl;
	for (int i = 0; i < size; i++) {
		int input;
		
		cin >> input;
		inputVec.push_back(input);
	}
	
	vector<int>& newSort=merge(inputVec);
	vector<int>::iterator it;
	cout << "Sorted Elements" << endl;
	for (it = newSort.begin(); it != newSort.end(); ++it)
		
		cout<< *it << endl;
	return 0;
}

