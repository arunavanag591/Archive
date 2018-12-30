#pragma once
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <sstream>
using namespace std;

class Palindrome
{
public:
	Palindrome();
	~Palindrome();

	static string format(string& word, string& combine) { // alternative formating option
		word.erase(std::remove(word.begin(), word.end(), '.'), word.end());
		combine = ""; // creates an empty string
		int n;
		int wcount = 0;
		n = word.length();
		for (int i = 0; i < n; i++) {
			word[i] = toupper(word[i]); // makes all letters capital
			if (word[i] != ' ')          // while there are no spaces, add to the empty string
				combine = combine + word[i];
			wcount++;
		}
		return combine;
	}

	static bool isPalindrome(std::string str)
	{
		int n = str.length();
		str.resize(remove_if(str.begin(), str.end(), [](char x) {return !isalnum(x) && !isspace(x); }) - str.begin());
		for (int i = 0; i < n; i++) {
			str[i] = toupper(str[i]); // makes all letters capital

		}
		//string combine;
		//format(str,combine);
		if (std::equal(str.begin(), str.begin() + str.length() / 2, str.rbegin()))
			return true;
		else
			return false;
	}
	void checkPalindrome();
};

class intReverse {

public:
	int reverse(int x) {

		long long result = 0;
		bool isNeg = x > 0 ? false : true;
		x = abs(x);
		while (x != 0) {
			result = result * 10 + x % 10;
			x = x / 10;
		}
		if (isNeg) {
			result *= -1;
		}
		if (result > INT_MAX || result < INT_MIN)
		{
			return 0;
		}
		else
		{
			return (int)result;
		}

	}
};
