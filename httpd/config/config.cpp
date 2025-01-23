/* -- config.cpp -- */

#include "../includes/configParser.hpp"

bool ConfigurationParser::extractErrorPages(ifstream& file, int& currentLineNumber) {
	string line;
	bool foundValidErrorPage = false;

	while (getline(file, line) && ++currentLineNumber) {
		if (LineIsCommentOrEmpty(line)) continue ;

		int CurrentIndentLevel = getIndentLevel(line);
		if (CurrentIndentLevel != 2)  return FileSeekg(file, line, currentLineNumber,foundValidErrorPage );
			
		clear_kv(kv);
		if (!verifyLineFormat(line, 1)) return false;
		CHECK_ALL_DIGITS(kv.key);
		VALIDATE_NUMERIC_RANGE(kv.key, 399, 600);
		currentServer.errorPages[value] = kv.value;
		foundValidErrorPage = true;
	}
	return foundValidErrorPage;
}

bool ConfigurationParser::extractClientMaxBodySizeValue(key_value& k_v) {
	VALIDATE_KV("client_max_body_size");
	
	CHECK_ALL_DIGITS(k_v.value);
	VALIDATE_NUMERIC_RANGE(k_v.value, 0, MAX_BODY_SIZE);
	currentServer.maxBodySize = value;
	return true;	
}

bool ConfigurationParser::extractServerNamesValue(key_value& k_v) {
	VALIDATE_KV("server_names");

	string parsed, input = k_v.value;
	stringstream input_ss(input);

	while (getline(input_ss, parsed, ' ')) {
		if (parsed.empty()) return false;
		currentServer.serverNames.push_back(parsed);
	}
	return true;
}

bool ConfigurationParser::isValidPortSegment(const string& segment) {
	if (segment.empty()) return false;

	CHECK_ALL_DIGITS(segment);
	VALIDATE_NUMERIC_RANGE(segment, 1023, 65536);
	return true;
}

bool ConfigurationParser::extractPortValue(key_value& k_v) {
	/* 1024 -> 65535 : Registred ports. */
	VALIDATE_KV("ports");
	// if (kv.key != "ports") return false;
	
	string parsed, input = k_v.value;
	stringstream input_ss(input);
	
	while (getline(input_ss, parsed, ',')) {
		if (!isValidPortSegment(parsed)) return false;
		currentServer.ports.push_back(parsed);
	}
	return true;
}

bool ConfigurationParser::isValidIPSegment(const string& segment) {
	if (segment.empty()) return false;

	CHECK_ALL_DIGITS(segment);
	VALIDATE_NUMERIC_RANGE(segment, -1, 256);
	return true;
}

bool ConfigurationParser::extractHostKey(key_value& k_v) { 
	VALIDATE_KV("host");

	int currentCountOfSubnets = 0;
	string parsed, input = k_v.value;
	stringstream input_ss(input);
	while (getline(input_ss, parsed, '.') && ++currentCountOfSubnets) {
		if (!isValidIPSegment(parsed)) return false;
		if (currentCountOfSubnets > 4) return false;
	}
	currentServer.host = k_v.value;
	return true;
}

bool ConfigurationParser::LineIsCommentOrEmpty(string& line) {
	if (line.empty()) return true;
	size_t firstNoSpaceTab = line.find_first_not_of(" \t");

	if (firstNoSpaceTab == string::npos) return true;
	string trimCheck = line.substr(firstNoSpaceTab);
	if (trimCheck[0] == '#') return true;
	return false;
}

bool ConfigurationParser::verifyLineFormat(string& line, int indentLevel) {
	if (line.empty()) return true;

	size_t firstNoTab = line.find_first_not_of('\t');

	if (firstNoTab == string::npos) return true;

	string trimmedLine = line.substr(firstNoTab);
	if (trimmedLine[0] == '#')
		return true;
	if (indentLevel == 1) {
		return isValidSecondLevel(trimmedLine);
	}
	return true;
}

bool ConfigurationParser::isValidSecondLevel(string& line) {
	/* nbdaw with num of : */
	size_t colonCount = 0;
	for (size_t i = 0; i < line.length(); ++i) {
		if (line[i] == ':')
	 		colonCount++;
	}
	if (colonCount != 1 && (line.find("return:") == string::npos)) return false;

	/* na5do pos of colona */
	size_t colonPosition = line.find(':');
	/* nshofo l key */
	kv.key = line.substr(0, colonPosition);
	if (kv.key.empty() || kv.key.find(' ') != string::npos || kv.key.find('\t') != string::npos)
		return false;
	//if (kv.key == "error_pages") return true;
	/* daba value dial dak key */
	kv.value = line.substr(colonPosition+1);
	size_t firstNoSpace = kv.value.find_first_not_of(" \t");
	size_t lastNoSpace = kv.value.find_last_not_of(" \t");
	if ((kv.value.empty() || firstNoSpace == string::npos) && kv.key == "error_pages") return true;
	if ((kv.value.empty() || firstNoSpace == string::npos) && kv.key == "cgi_path") return true;
	if (kv.value.empty() || firstNoSpace == string::npos) return false;
	if (firstNoSpace != string::npos && lastNoSpace  != string::npos)
		kv.value = kv.value.substr(firstNoSpace, lastNoSpace - firstNoSpace + 1);
	return true;
}

void ConfigurationParser::syntaxError(int currentLineNumber, const string& errorMessage) {
	ostringstream oss;
	oss << currentLineNumber;
	throw runtime_error("Error on line " + oss.str() + ": " + errorMessage);
}

void ConfigurationParser::clear_kv(key_value& kv) {
	kv.key.clear();
	kv.value.clear();
}
