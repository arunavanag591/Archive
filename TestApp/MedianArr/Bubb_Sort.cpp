#include "Bubb_Sort.h"
using namespace std;

Bubb_Sort::Bubb_Sort()
{
	static string exc = "Invalid entry";
}

Bubb_Sort::~Bubb_Sort()
{
}

void Bubb_Sort::bubbleSort(int a[], int n) {

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (a[j]>a[j + 1])
			{
				swap(a[j], a[j + 1]);
			}
		}
	}

}

void Bubb_Sort::checkBubbSort(int arr[], int n) { // this goes into main.cpp
	
	bubbleSort(arr, n);
	cout << "Sorted Array is: " << endl;
	for (int i = 0; i < 5; i++)
	{
		cout << arr[i] << endl;
	}

}

void Bubb_Sort::inputBubbSort() {
	int arr[] = { 22, 55, 11, 9, 99 };
	int n = 5;
	checkBubbSort(arr, n);
};

void Bubb_Sort::expPoints() {
	int a = 50;
	cout << endl << a;
	int *b = &a;
	*b = 100;
	cout << endl << a;
	int **c = &b;
	**c = 200;
	cout << endl << a << endl;
};

int Bubb_Sort::findFactorial(int x) {
	if (x == 1) {
		return 1;
	}
	else {

		return x*findFactorial(x - 1);
	}
}