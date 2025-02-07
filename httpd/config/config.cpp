/* -- config.cpp -- */

#include "../includes/configParser.hpp"

/*
 * Extract and validates error pages from the config file.
 *
 * haka khassha tkun:
 * error_pages:
 * 		404: path_to/404.htmt
 * 		error_code: fin kain
 * 	
 * 	Parameters:
 * 		file: Input file stream positioned at start of error_pages block.
 * 		currentLineNumber: zid ? (NB: modified by the function)
 * 	
 * 	Returns:
 * 		true if at least one error page was found
 *
 * 	Notes:
 * 		- Error codes should be between 399-600
 * 		- Path must be valide as paths
 */
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

/*
 * Extract and validates client max body size from the config file.
 *
 * haka khassha tkun:
 * client_max_body_size: 10485760 # 10MB in bytes
 * 	
 * 	Parameters:
 * 		kv: struct 3endha two members: key&value
 * 		(FHAD LCASE: key == client_max_body_size && value == correct_number)
 * 	
 * 	Returns:
 * 		true if value is valid, false otherwise
 *
 * 	Notes:
 * 		- Must be numeric
 * 		- Must be between 0 and MAX_BODY_SIZE
 * 		_ Only one entry allowed per server block
 */
bool ConfigurationParser::extractClientMaxBodySizeValue(key_value& k_v) {
	VALIDATE_KV("client_max_body_size");
	
	CHECK_ALL_DIGITS(k_v.value);
	VALIDATE_NUMERIC_RANGE(k_v.value, 0, MAX_BODY_SIZE);
	currentServer.maxBodySize = value;
	return true;	
}

/*
 * Extract and validates server_names from the config file.
 *
 * haka khassha tkun:
 * server_names: example.com example1.com localhost 	
 * 	Parameters:
 * 		kv: key-value pairs fiha server names
 * 	
 * 	Returns:
 * 		true if <-if
 *
 * 	Notes:
 * 		- Multiple names are space separated
 * 		_ Empty names are not allowed
 */
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

/*
 * Extract and validates ports from the config file.
 *
 * haka khassha tkun:
 * ports: 8080,8081,...,...
 * 	Parameters:
 * 		kv: key-value pairs fiha ports
 * 	
 * 	Returns:
 * 		true if the port is correct 🧐
 *
 * 	Notes:
 * 		- Multiple ports are ',' separated
 * 		- Each port must be between 1024-65535 (registered ports)
 * 		- No duplicate ports are allowed within the same server block
 */
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

bool ConfigurationParser::isValidPortSegment(const string& segment) {
	if (segment.empty()) return false;

	CHECK_ALL_DIGITS(segment);
	VALIDATE_NUMERIC_RANGE(segment, 1023, 65536);
	return true;
}

/*
 * Extract and validates host from the config file.
 *
 * haka khassha tkun:
 * host: 127.0.0.1
 * 	Parameters:
 * 		kv: key-value pairs fiha host
 * 	 
 * 	Returns:
 * 		true if the host is correct 🧐
 *
 * 	Notes:
 * 		- Must be valid IPv4 address
 * 		- Each segment must be 0-255
 * 		- Must have exactly 4 segment
 */
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

bool ConfigurationParser::isValidIPSegment(const string& segment) {
	if (segment.empty()) return false;

	CHECK_ALL_DIGITS(segment);
	VALIDATE_NUMERIC_RANGE(segment, -1, 256);
	return true;
}

/*
 * Determines if a line should be skipped during parsing.
 *
 * Params:
 * 		Line: Li ghadi yt checka.
 * 
 * Returns:
 * 		bool: true if the line is comment or empty, false otherwise
 *
 * 	Skips:
 * 		- Empty lines
 * 		- Lines starting by # (comments)
 * 		- Lines with only whitespaces
 */
bool ConfigurationParser::LineIsCommentOrEmpty(string& line) {
	if (line.empty()) return true;
	size_t firstNoSpaceTab = line.find_first_not_of(" \t");

	if (firstNoSpaceTab == string::npos) return true;
	string trimCheck = line.substr(firstNoSpaceTab);
	if (trimCheck[0] == '#') return true;
	return false;
}

/*
 * Verify line format and exctract key-value pairs
 *
 * Params:
 * 		Line: Li ghadi yt checka.
 * 		indentLevel: dial Line (in tabs)
 * 
 * Returns:
 * 		bool: true if the line format is correct, false otherwise
 */
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

/*
 * Validate Second-Level Lines (bime3na: currentIndentLevel == 2, ou kainin shi cases kan5dem biha)
 *
 * Params:
 * 		Line: Li ghadi yt valida
 * 
 * Returns:
 * 		bool: true if the line format is valid, false otherwise
 *
 * 	Validation Rules:
 * 		- Exactly colona (:) per line (except for special case like "return")
 * 		- key cannot be empty (no spaces or smth also)
 * 		- Value cannot be empty
 * 		- Trimes leading/trailing whitspaces from the value (bash ta la ja kteb key:     8080    )
 */
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

/*
 * Generate an error message and throws an exception for parsing failures
 *
 * Params:
 * 		Current Line Number: no need right ? 
 * 		custom error message
 */
void ConfigurationParser::syntaxError(int currentLineNumber, const string& errorMessage) {
	ostringstream oss;
	oss << currentLineNumber;
	throw runtime_error("Error on line " + oss.str() + ": " + errorMessage);
}

/*
 * Clears Key-Value pair
 *
 * Params:
 * 		kv: key-value pair to clear
 */
void ConfigurationParser::clear_kv(key_value& kv) {
	kv.key.clear();
	kv.value.clear();
}
