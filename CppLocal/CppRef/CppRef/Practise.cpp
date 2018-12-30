#include "prac.h"

User::User() {
	location = 0;
	maxsize = 0;
	count = 0;
}
User::~User() {
}

Hash::Hash() {

}
Hash::~Hash() {

}
//TODO: take care of the person class and implement the necessary data structure
Person::Person() {}
Person::~Person() {}

void User::longestPalindrome(char a[]) {
	char* ptr = a;
	for (int i = 0; i < strlen(a) - 1; i++) {
		int left = i;
		int right = i;
		int count = 0;
		while (left > 0) {
			if (a[left--] != a[right++]) {
				break;
			}
			count++;
		}
		if (count > maxsize) {
			maxsize = count;
			location = i;
		}
	}
	cout << maxsize << " @ " << location << endl;
	int start = location - maxsize;
	int end = location + maxsize;
	for (int i = start + 1; i < end; i++) {
		cout << a[i];
	}
	cout << endl;
}

void User::reverseSentence(string s) {
	stringstream stream(s);
	//stringstream is;
	string word;
	vector<string> words;
	while (stream >> word) {	//inserting each word from the stringstream
		words.push_back(word);
	}
	
	reverse(words.begin(), words.end());
	for (int i=0; i < words.size(); ++i) {
		
		cout << words[i] << " ";
	}
	cout << endl;
}

//Implementing Hash Map

int Hash::Hashed(string key) {
	//TODO: Implement Hash Map
	return 0;
}



string User::longestPalindromeStr(string s) {
	size_t len = s.size();
	int P[50][50];
	memset(P, 0, len*len * sizeof(int));
	int maxL = 0, start = 0, end = 0;
	for (int i = 0; i< s.size(); i++)
	{
		for (int j = 0; j<i; j++)
		{
			P[j][i] = (s[j] == s[i] && (i - j<2 || P[j + 1][i - 1]));
			if (P[j][i] && maxL < (i - j + 1))
			{
				maxL = i - j + 1;
				start = j;
				end = i;
			}
		}
		P[i][i] = 1;
	}
	return s.substr(start, end - start + 1);
}

