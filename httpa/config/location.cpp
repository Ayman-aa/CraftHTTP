/* -- location.cpp --*/

#include "../includes/configParser.hpp"

bool ConfigurationParser::extractLocationInfos(ifstream& file, int& currentLineNumber, Location& location) {
	string line;
	bool findLoc = false;

	while (getline(file, line) && ++currentLineNumber) {
		int currentIndentLevel = getIndentLevel(line);

		if (LineIsCommentOrEmpty(line)) continue;
		if (currentIndentLevel != 2) return FileSeekg(file, line, currentLineNumber, findLoc);

		clear_kv(kv);

		CHECK_AND_EXTRACT_BOOL("autoindex:", extractAutoIndexValue);
		CHECK_AND_EXTRACT_CONTAINER("index:", location.index, extractIndexValues);
		CHECK_AND_EXTRACT_STR("return:", location.redirection_return, extractReturnValue);
		CHECK_AND_EXTRACT_STR("upload_path:", location.upload_path, extractUploadPath);
		CHECK_AND_EXTRACT_STR("root:", location.root, extractRootValue);
		CHECK_AND_EXTRACT_CONTAINER("allowed_methods", location.allow_methods, extractAllowedMethods);
		if (line.find("cgi_path:") != string::npos) {
			if (!location.cgi_path.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractCgiPath(file, location, currentLineNumber)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
			continue;
		}
		if (line.find("server:") != string::npos) continue ;
		else syntaxError(currentLineNumber, SYNTAX_ERROR); 
	}
	return findLoc;
}

bool ConfigurationParser::servLocationLine(key_value& k_v, Location& location) {
	if (k_v.value.empty()) return false;
	if (k_v.key != "location") return false;

	if (k_v.value[0] != '/') return false;
	if (k_v.value.find("..") != string::npos) return false;
	if (k_v.value.find("//") != string::npos) return false;
	location.path = kv.value;
	return true;	
}
bool ConfigurationParser::extractUploadPath(key_value& k_v, Location& location) {
	if (k_v.value.empty() || k_v.key != "upload_path")	return false;

	if (k_v.value.find(' ') != string::npos) return false;
	location.upload_path = k_v.value;
	return true;
}

bool ConfigurationParser::isValidCgiKey(const string& method) {
	/* std++98 sir 4er t7awa */
	return (method == "php" || method == "rb" || method == "py");
}

bool ConfigurationParser::extractCgiPath(ifstream& file, Location& location, int& currentLineNumber) {
	string line;
	bool findPath = false;
	set<string> cgi;

	while (getline(file, line) && ++currentLineNumber) {
		if (LineIsCommentOrEmpty(line)) continue;
		int CurrentIndentLevel = getIndentLevel(line);
		if (CurrentIndentLevel != 3) return FileSeekg(file, line, currentLineNumber, findPath);
		clear_kv(kv);
		if (!verifyLineFormat(line, 1)) return false;
		if (!isValidCgiKey(kv.key)) return false;
		if (!cgi.insert(kv.key).second)
			return false;
		if (kv.value.empty()) return false;

		location.cgi_path[kv.key] = kv.value;
		findPath = true;
	}
	return findPath;
}

bool ConfigurationParser::extractRootValue(key_value& k_v, Location& location) {
	if (k_v.value.empty() || k_v.key != "root") return false;

	if (k_v.value.find(' ') != string::npos) return false;
	location.root = k_v.value;
	return true;
}

bool ConfigurationParser::extractReturnValue(key_value& k_v, Location& location) {
	if (k_v.value.empty() || k_v.key != "return") return false;
	
	if (k_v.value.find("..") != string::npos) return false;
	if (k_v.value.find(' ') != string::npos) return false;
	location.redirection_return = k_v.value;
	return true;
}

bool ConfigurationParser::isValidIndex(const string& index) {
	if (index.find(' ') != string::npos) return false;
	/*
		* if (index.find("../") != string::npos) return false;
		*if (index.find('/') != string::npos) return false;
	*/

	if (index.find('&') != string::npos) return false;
    if (index.find('|') != string::npos) return false;
    if (index.find(';') != string::npos) return false;
    if (index.find('$') != string::npos) return false;

	return true;
}

bool ConfigurationParser::extractIndexValues(key_value& k_v, Location& location) {
	if (k_v.value.empty() || k_v.key != "index") return false;

	string parsed, input = k_v.value;
	istringstream iss(input);

	while (getline(iss, parsed, ',')) {
		if (!isValidIndex(parsed)) return false;
		location.index.push_back(parsed);
	}
	return true;
}

bool ConfigurationParser::isValidMethod(const string& method) {
	/* std++98 sir 4er t7awa */
	 return (method == "GET" || method == "POST" || method == "DELETE");
}

bool ConfigurationParser::extractAllowedMethods(key_value& k_v, Location& location) {
	if (k_v.value.empty() || k_v.key != "allowed_methods") return false;
	
	set<string> uniqueMethods;	
	string parsed, input = k_v.value;
	istringstream iss(input);
	
	while (getline(iss, parsed, ',')) {
		if (!isValidMethod(parsed)) return false;
		location.allow_methods.push_back(parsed);
		/* nchoufou duplicate hna nit */
		if (!uniqueMethods.insert(parsed).second)
			return false;
	}
	return true;
}

bool ConfigurationParser::extractAutoIndexValue(key_value& k_v, Location& location) {
	if (k_v.value.empty() || k_v.key != "autoindex") return false;

	if (k_v.value != "on" && k_v.value != "off") return false;
	location.autoindex = (k_v.value == "on");
	return true;
}