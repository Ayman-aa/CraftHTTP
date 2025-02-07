/* -- location.cpp --*/

#include "ConfigurationParser.hpp"

bool ConfigurationParser::extractLocationInfos(std::ifstream& file, int& currentLineNumber, Location& location) {
	string line;
	bool findLoc = false;

	while (getline(file, line) && ++currentLineNumber) {
		int currentIndentLevel = getIndentLevel(line);

		if (LineIsCommentOrEmpty(line)) continue;
		if (currentIndentLevel != 2) return FileSeekg(file, line, currentLineNumber, findLoc);

		clear_kv(kv);

		CHECK_AND_EXTRACT_LOCATION_BOOL("autoindex:", extractAutoIndexValue);
		CHECK_AND_EXTRACT_LOCATION("index:", location.index, extractIndexValues);
		CHECK_AND_EXTRACT_LOCATION("return:", location.redirection_return, extractReturnValue);
		CHECK_AND_EXTRACT_LOCATION("upload_path:", location.upload_path, extractUploadPath);
		CHECK_AND_EXTRACT_LOCATION("root:", location.root, extractRootValue);
		CHECK_AND_EXTRACT_LOCATION("allowed_methods", location.allow_methods, extractAllowedMethods);

		if (line.find("cgi_path:") !=std::string::npos) {
			if (!location.cgi_path.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractCgiPath(file, location, currentLineNumber)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
			continue;
		}
		if (line.find("server:") !=std::string::npos) continue ;
		else syntaxError(currentLineNumber, SYNTAX_ERROR); 
	}
	return findLoc;
}

/* TODO: 3AWED T2KD MN SPACE LAY RDI 3LIK, ZE3MA WASH 5ASSEK T CHECKI 3LA WASH VALUE FIHA ' ' WLA LA*/
bool ConfigurationParser::servLocationLine(key_value& k_v, Location& location) {
	VALIDATE_KV("location");
	CHECK_STRING_FORMAT(k_v.value, "..");
	CHECK_STRING_FORMAT(k_v.value, "//");
	if (k_v.value[0] != '/') return false;
	location.path = kv.value;
	return true;	
}

bool ConfigurationParser::extractUploadPath(key_value& k_v, Location& location) {
	VALIDATE_KV("upload_path");
	CHECK_STRING_FORMAT(k_v.value, ' ');
	location.upload_path = k_v.value;
	return true;
}

bool ConfigurationParser::isValidCgiKey(const std::string& method) {
	/* std++98 sir 4er t7awa */
	return (method == "php" || method == "rb" || method == "py");
}

bool ConfigurationParser::extractCgiPath(std::ifstream& file, Location& location, int& currentLineNumber) {
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
	VALIDATE_KV("root");
	CHECK_STRING_FORMAT(k_v.value, ' ');
	location.root = k_v.value;
	return true;
}

bool ConfigurationParser::extractReturnValue(key_value& k_v, Location& location) {
	VALIDATE_KV("return");
	CHECK_STRING_FORMAT(k_v.value, "..");
	CHECK_STRING_FORMAT(kv.value, ' ');
	location.redirection_return = k_v.value;
	return true;
}

bool ConfigurationParser::isValidIndex(const std::string& index) {
	CHECK_STRING_FORMAT(index, ' ');
	CHECK_INVALID_CHARS(index);
	return true;

	/*
		*if (index.find("../") !=std::string::npos) return false;
		*if (index.find('/') !=std::string::npos) return false;
	*/
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

bool ConfigurationParser::isValidMethod(const std::string& method) {
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
	VALIDATE_KV("autoindex");
	if (k_v.value != "on" && k_v.value != "off") return false;
	location.autoindex = (k_v.value == "on");
	return true;
}
