/* -- location.cpp --*/

#include "../includes/configParser.hpp"

void printLocation(const Location& location);

bool ConfigurationParser::servLocationLine(key_value& k_v, Location& location) {
	if (k_v.value.empty()) return false;
	if (k_v.key != "location") return false;

	if (k_v.value[0] != '/') return false;
	if (k_v.value.find("..") != string::npos) return false;
	if (k_v.value.find("//") != string::npos) return false;
	location.path = kv.value;
	return true;	
}

bool ConfigurationParser::extractLocationInfos(ifstream& file, int& currentLineNumber, Location& location) {
	string line;

	location.autoindex = false;
	bool findLoc = false;

	while (getline(file, line) && ++currentLineNumber) {
		int currentIndentLevel = getIndentLevel(line);

		if (LineIsCommentOrEmpty(line)) continue;
		if (currentIndentLevel != 2) {
			/*Line li b4ina ndriou bih rojo3 b zaman: */
			FileSeekg(file, line, currentLineNumber);
			return findLoc;
		}

		clear_kv(kv);
		if (line.find("autoindex:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractAutoIndexValue(kv, location)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
		}
		else if (line.find("allowed_methods:") != string::npos) {
			if (!location.allow_methods.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractAllowedMethods(kv, location)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
		}
		else if (line.find("index:") != string::npos) {
			if (!location.index.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractIndexValues(kv, location)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
		}
		else if (line.find("return:") != string::npos) {
			if (location.redirection_return != "") syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractReturnValue(kv, location)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
		}
		else if (line.find("upload_path:") != string::npos) {
			if (location.upload_path != "") syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractUploadPath(kv, location)) syntaxError(currentLineNumber, SYNTAX_ERROR);
		}
		else if (line.find("root:") != string::npos) {
			if (location.root != "") syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractRootValue(kv, location)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
		}
		else if (line.find("cgi_path:") != string::npos) {
			if (!location.cgi_path.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			if (!extractCgiPath(file, location, currentLineNumber)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			findLoc = true;
		}
		else if (line.find("server:") != string::npos) continue ;
		else syntaxError(currentLineNumber, SYNTAX_ERROR); 
	}
	return findLoc;
}

bool ConfigurationParser::extractUploadPath(key_value& k_v, Location& location) {
	if (k_v.value.empty())	return false;
	if (k_v.key != "upload_path") return false;

	if (k_v.value.find(' ') != string::npos) return false;
	location.upload_path = k_v.value;
	return true;
}

bool ConfigurationParser::isValidCgiKey(const string& method) {
	/* std++98 sir 4er t7awa */
	static  set<string> cgi;
	static bool init = false;

	if (!init) {
		cgi.insert("php");
		cgi.insert("rb");
		cgi.insert("py");
		init = true;
	}	
	return cgi.find(method) != cgi.end();
}

bool ConfigurationParser::extractCgiPath(ifstream& file, Location& location, int& currentLineNumber) {
	string line;
	bool findPath = false;
	set<string> cgi;

	while (getline(file, line) && ++currentLineNumber) {
		if (LineIsCommentOrEmpty(line)) continue;
		int CurrentIndentLevel = getIndentLevel(line);
		if (CurrentIndentLevel != 3) {
			FileSeekg(file, line, currentLineNumber);
			return findPath;
		}
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
	if (k_v.value.empty()) return false;
	if (k_v.key != "root") return false;

	if (k_v.value.find(' ') != string::npos) return false;
	location.root = k_v.value;
	return true;
}

bool ConfigurationParser::extractReturnValue(key_value& k_v, Location& location) {
	if (k_v.value.empty()) return false;
	if (k_v.key != "return") return false;
	
	if (k_v.value.find("..") != string::npos) return false;
	if (k_v.value.find(' ') != string::npos) return false;
	location.redirection_return = k_v.value;
	return true;
}

bool ConfigurationParser::isValidIndex(const string& index) {
	vector<char> invalidChars;

	invalidChars.push_back('&');
	invalidChars.push_back('|');
	invalidChars.push_back(';');
	invalidChars.push_back('$');

	if (index.find(' ') != string::npos) return false;
	if (index.find("../") != string::npos) return false;
	if (index.find('/') != string::npos) return false;

	for (size_t i = 0; i < 3 ; i++)
		if (index.find(invalidChars[i]) != string::npos) return false;
	return true;
}

bool ConfigurationParser::extractIndexValues(key_value& k_v, Location& location) {
	if (k_v.value.empty()) return false;
	if (k_v.key != "index") return false;

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
	static  set<string> methods;
	static bool init = false;

	if (!init) {
		methods.insert("GET");
		methods.insert("POST");
		methods.insert("DELETE");
		init = true;
	}	
	return methods.find(method) != methods.end();
}

bool ConfigurationParser::extractAllowedMethods(key_value& k_v, Location& location) {
	if (k_v.value.empty()) return false;
	if (k_v.key != "allowed_methods") return false;
	
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
	if (k_v.value.empty()) return false;
	if (k_v.key != "autoindex") return false;

	if (k_v.value != "on" && k_v.value != "off") return false;
	location.autoindex = (k_v.value == "on");
	return true;
}