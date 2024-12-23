#include <iostream>

using namespace std;

int main(int ac, char *av[]) 
{
	(void)ac; /* ma3ndi mandir bik ya zebi */
	string value = av[1];

	if (value.find('./..') != string::npos) {
		cout << "[!] Had check: if (value.find('./..') != string::npos)" << endl;
		return 0;
	}
	/* Safy, Nadodi */
	/* if (value[0].find('/') != string::npos) {
		cout << "[!] Had check: if (value[0].find('/') != string::npos)" << endl;
		return 0;
	} */
	return 0;
}
