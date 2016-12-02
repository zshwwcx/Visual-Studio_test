#include <iostream>
#include <cstring>

using namespace std;
#pragma warning(disable:4996)
char* getname() {
	char temp[80];
	cout << "Please input the name you want to create:" << endl;
	cin >> temp;
	char* pn =new char;
	strcpy(pn, temp);
	return pn;
}

int main() {
	char* name;
	name = getname();
	cout << "The name you create is :" << name << "__" << (int *)name << endl;
	//cout << (int *)"hello the test" << endl;
	cout << "Quit?(y/n)" << endl;
	while (1) {
		if (getchar() == 'y') {
			exit(0);
		}
	}
	return 0;
}