/* -- main.cpp -- */

#include "SmartPtr.hpp"
#include <iostream>
#include <string>

using namespace std;

int main() {
	SmartPtr<string> sp(new string("Hello Abdurrahman"));
	cout << *sp << std::endl;
	cout << sp->length() << std::endl;
}
