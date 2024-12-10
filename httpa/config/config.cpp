/* -- config.cpp -- */

#include "configParser.hpp"

key_value kv;

ConfigurationParser::ConfigurationParser(string& filePath) {
	ifstream file(filePath.c_str());
	if (!file.is_open())
		throw runtime_error("failed to open file: " + filePath);
	ConfigurationParser::load(file);
}

void ConfigurationParser::load(ifstream& file) {
	string line;
	int currentLineNumber = 0;

	while (getline(file, line) && ++currentLineNumber) {
		if (!isValidRootLevel(line)) syntaxError(currentLineNumber);
		while (getline(file, line) && ++currentLineNumber) {
			int currentIndentLevel = getIndentLevel(line);
			if (currentIndentLevel != 1) syntaxError(currentLineNumber);
			
			if (line.find("host:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (extractHostKey(kv)) this->host = kv.currentParsedValue;
				else syntaxError(currentLineNumber);
			}
			else if (line.find("ports:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
			}
			else if (line.find("server_names:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
			}
			else if (line.find("error_pages:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
			}
			else if (line.find("client_max_body_size:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
			}
		}
	}
}

bool ConfigurationParser::isValidRootLevel(string& line) {
	int rootIndentLevel = getIndentLevel(line);
		return rootIndentLevel == 0 && line == "server:";
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

bool ConfigurationParser::isValidIPSegment(const string& segment) {
	if (kv.key != "host") return false;
	if (segment.empty()) return false;

	for (size_t i = 0; i < segment.length(); i++)
		if (!isdigit(segment[i])) return false;
	
	int value = 0;
	istringstream iss(segment);
	iss >> value;
	return (value >= 0 && value <= 255);
}

bool ConfigurationParser::extractHostKey(key_value k_v) {
	int currentCountOfSubnets = 0;
	string parsed, input = k_v.value;
	stringstream input_ss(input);
	while (getline(input_ss, parsed, '.') && ++currentCountOfSubnets) {
		cout << "Parsed Ya Zebi: " << parsed << endl;
		if (!isValidIPSegment(parsed)) return false;
		if (currentCountOfSubnets > 4) return false;
	}
	k_v.currentParsedValue = k_v.value;
	return true;
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
		return isValidSecondLevel(trimmedLine);

	return true;
}

bool ConfigurationParser::isValidSecondLevel(string& line) {
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
	kv.key = line.substr(0, colonPosition);
	if (kv.key.empty() || kv.key.find(' ') != string::npos || kv.key.find('\t') != string::npos)
		return false;
	cout << "key: " << kv.key << endl;
	if (kv.key == "error_pages") return true;
	/* daba value dial dak key */
	kv.value = line.substr(colonPosition+1);
	size_t firstNoSpace = kv.value.find_first_not_of(" \t");
	size_t lastNoSpace = kv.value.find_last_not_of(" \t");
	if (kv.value.empty() || firstNoSpace == string::npos) return true;
	if (firstNoSpace != string::npos && lastNoSpace  != string::npos)
		kv.value = kv.value.substr(firstNoSpace, lastNoSpace - firstNoSpace + 1);
	cout << "value: " << kv.value<< endl;
	return true;
}

void ConfigurationParser::syntaxError(int currentLineNumber) {
	ostringstream oss;
	oss << currentLineNumber;
	throw runtime_error("error: syntax error at line " + oss.str());
}
