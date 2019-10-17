#include <iostream>
#include "vector.h"

int main() {
	vector<int>v1;
	for (int i = 1; i < 11; i++) {
		v1.push_back(i);
	}

	vector<int>v2;
	for (int i = 1; i < 5; i++) {
		v2.push_back(i);
	}

	v1 = v2;
	for (int i = 0; i < v1.size(); i++) {
		std::cout << v1[i] << '\n';	
	}

	return 0;
}