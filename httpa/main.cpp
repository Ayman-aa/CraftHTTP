/* -- main.cpp -- */

# include <iostream>
# include "yaml/yaml.hpp"

using namespace std;

/* global var */
const char *filePath;
char *error;

int main(int ac, char *av[])
{
	if (ac != 2) {
		if (ac > 2) {
			fprintf(stderr, "[Error] Invalid number of arguments. Usage: %s <config_file>\n", av[0]);
			return 1;
		}
		fprintf(stderr, "[Warning] No configuration file provided. Proceeding with default configuration.\n");
		filePath = "conf/conf.yaml";
	}
	else
		filePath = av[1];
	fprintf(stdout, "filePath: %s\n", filePath);

	YAML::yaml fileParser(filePath);
}
