/* -- location.cpp --*/

#include "configParser.hpp"

bool ConfigurationParser::servLocationLine(key_value& k_v) {
	if (kv.value.empty()) return false;
	if (kv.key != "location") return false;
	(void)k_v;
	return true;	
}

/*bool ConfigurationParser::extractLocationInfos(ifstream& file, int& currentLineNumber) {
	string line;

	while (getline(file, line)) {
		
	}
}*/
