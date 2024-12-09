/* -- config.cpp -- */

#include "configParser.hpp"

kv keyValue;

ConfigurationParser::ConfigurationParser(string& filePath) {
	ifstream file(filePath.c_str());
	if (!file.is_open())
		throw runtime_error("failed to open file: " + filePath);
	ConfigurationParser::load(file);
}

void ConfigurationParser::load(ifstream& file) {
	string line;
	int currentLineNumber = 0;

	while (getline(file, line)) {
		if (!isValidRootLevel(line)) syntaxError(currentLineNumber);
	}
}

bool ConfigurationParser::isValidRootLevel(string& line) {
		return line == "server:";
}

int ConfigurationParser::getIndentLevel(const string& line) {
	int IndentLevel = 0; /* must be tabs nor space */
	char tab = '\t';
	
	for (size_t i = 0; i < line.length(); i++) {
		if (line[i] == tab)
			IndentLevel++;
		else 
			break;
	}
	return IndentLevel;
}

bool ConfigurationParser::verifyLineFormat(string& line, int indentLevel) {
	if (line.empty())
		return true;
	size_t firstNoTab = line.find_first_not_of('\t');
	if (firstNoTab == string::npos)
		return true;
	string trimmedLine = line.substr(firstNoTab);
	if (trimmedLine[0] == '#')
		return true;
	if (indentLevel == 1)
		return isValidSecondLevel(trimmedLine, keyValue.key, keyValue.value);

	return true;
}

bool ConfigurationParser::isValidSecondLevel(string& line, string& key, string& value) {
	/* nbdaw with num of : */
	size_t colonCount = 0;
	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] == ':')
			colonCount++;
	}
	if (colonCount != 1) return false;

	/* na5do pos of colona */
	size_t colonPosition = line.find(':');
	/* nshofo l key */
	key = line.substr(0, colonPosition);
	if (key.empty() || key.find(' ') != string::npos || key.find('\t') != string::npos)
		return false;
	/* daba value dial dak key */
	value = line.substr(colonPosition+1);
	size_t firstNoSpace = value.find_first_not_of(" \t");
	size_t lastNoSpace = value.find_last_not_of(" \t");
	if (value.empty() || firstNoSpace == string::npos) return true;
	if (firstNoSpace != string::npos && lastNoSpace  != string::npos)
		value = value.substr(firstNoSpace, lastNoSpace - firstNoSpace + 1);
	cout << "key: " << key << " | value: " << value<< endl;
	return true;
}

void ConfigurationParser::syntaxError(int currentLineNumber) {
	ostringstream oss;
	oss << currentLineNumber;
	throw runtime_error("error: syntax error at line " + oss.str());
}
