/* -- main.cpp -- */

# include <iostream>
# include "yaml/yaml.hpp"

using namespace std;

/* global var */
char *filePath;
char *error;

int main(int ac, char *av[])
{
	if (ac != 2) {
		if (ac > 2) {
			fprintf(stderr, "[Error] Invalid number of arguments. Usage: ./program_name <config_file>");
			return 1;
		}
		fprintf(stderr, "[Warning] No configuration file provided. Proceeding with default configuration.");
		filePath = "conf.yaml";
	}
}
