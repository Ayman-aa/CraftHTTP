/* -- location.cpp --*/

#include "ConfigurationParser.hpp"

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

		cout << "7na wesst l while, 3lash Line: "<< "'" << line << "'" << endl;
		if (LineIsCommentOrEmpty(line)) continue;
		if (currentIndentLevel != 2) {
			cout << "Line li b4ina ndriou bih rojo3 b zaman: " << "'" << line << "'" << endl;
			file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
			currentLineNumber--;
			return findLoc;
		}
		clear_kv(kv);

		if (line.find("autoindex:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber);
			if (!extractAutoIndexValue(kv, location)) syntaxError(currentLineNumber);
			findLoc = true;
			//printLocation(location);
		}
		else if (line.find("allowed_methods:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber);
			if (!extractAllowedMethods(kv, location)) syntaxError(currentLineNumber);
			findLoc = true;
			//printLocation(location);
		}
		else if (line.find("index:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber);
			if (!extractIndexValues(kv, location)) syntaxError(currentLineNumber);
			findLoc = true;
			//printLocation(location);
		}
		else if (line.find("return:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber);
			if (!extractReturnValue(kv, location)) syntaxError(currentLineNumber);
			findLoc = true;
			//printLocation(location);
		}
		else if (line.find("root:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber);
			if (!extractRootValue(kv, location)) syntaxError(currentLineNumber);
			findLoc = true;
			//printLocation(location);
		}
		else if (line.find("cgi_path:") != string::npos) {
			if (!verifyLineFormat(line, 1)) syntaxError(currentLineNumber);
			if (!extractCgiPath(file, location, currentLineNumber)) syntaxError(currentLineNumber);
			findLoc = true;
			//printLocation(location);
		}
		else if (line.find("server:") != string::npos) continue ;
		else syntaxError(currentLineNumber); 
	}
	return findLoc;
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
		cout << "Had line l5er fin 5litihi: " << line << endl;
		int CurrentIndentLevel = getIndentLevel(line);
		if (CurrentIndentLevel != 3) {
			file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
			currentLineNumber--;
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





// Helper function to print a vector if it exists
void printVector(const vector<string>& vec, const string& label) {
    if (!vec.empty()) {
        cout << "  " << label << ": ";
        for (vector<string>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
            cout << *it << " ";
        }
        cout << endl;
    } else {
        cout << "  " << label << ": Not set" << endl;
    }
}

// Helper function to print a map if it exists
void printMap(const map<string, string>& m, const string& label) {
    if (!m.empty()) {
        cout << "  " << label << ":" << endl;
        for (map<string, string>::const_iterator it = m.begin(); it != m.end(); ++it) {
            cout << "    " << it->first << " -> " << it->second << endl;
        }
    } else {
        cout << "  " << label << ": Not set" << endl;
    }
}

// Function to print the Location struct
void printLocation(const Location& location) {
    cout << "Location Details:" << endl;

    // Autoindex always exists
    cout << "  Autoindex: " << (location.autoindex ? "true" : "false") << endl;

    // Check and print string values
    cout << "  Root: " << (!location.root.empty() ? location.root : "Not set") << endl;
    cout << "  Path: " << (!location.path.empty() ? location.path : "Not set") << endl;
    cout << "  Redirection Return: " << (!location.redirection_return.empty() ? location.redirection_return : "Not set") << endl;
    cout << "  Upload Path: " << (!location.upload_path.empty() ? location.upload_path : "Not set") << endl;

    // Check and print vectors
    printVector(location.allow_methods, "Allowed Methods");
    printVector(location.index, "Index");

    // Check and print map
    printMap(location.cgi_path, "CGI Paths");
}

