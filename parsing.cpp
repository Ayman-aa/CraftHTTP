/* -- parsing.cpp -- */

#include <iostream>
#include <cstdio> /* fprintf(), ... */
#include <cctype> /* isdigit, .... */
#include <sstream>

int main(int ac, char *av[]) {
	using namespace std;

	if (ac != 2) {
		fprintf(stderr, "Usage: %s 'Host address you want to check'\n", av[0]);
		return 1;
	}
	string avv = av[1];
	string parsed,input=avv;
	stringstream input_stringstream(input);

	for (int i = 0; i < 4; i++) {	
		if (getline(input_stringstream,parsed,'.')) {
			 for (int i = 0; i < parsed.length(); i++) {
				 if (!isdigit(parsed[i])) {
					cout << "Error: " << parsed << endl;
					break ;
				 }
				 else cout << "Correct: " << parsed << endl;
			 }
		}
//		
	}
}
