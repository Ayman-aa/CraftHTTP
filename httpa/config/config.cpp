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
	this->maxBodySize = -1;

	while (getline(file, line) && ++currentLineNumber) {
		if (!isValidRootLevel(line)) syntaxError(currentLineNumber);
		while (getline(file, line) && ++currentLineNumber) {
			int currentIndentLevel = getIndentLevel(line);
			clear_kv(kv);
			cout << "Line jdid li rje3 houw li fih host: " << line << endl;

			if (LineIsCommentOrEmpty(line)) continue ;
			if (currentIndentLevel != 1) syntaxError(currentLineNumber);

			if (line.find("host:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (extractHostKey(kv))  {
					if (!this->host.empty()) syntaxError(currentLineNumber);
					this->host = kv.currentParsedValue;
					cout << "this->host: " << this->host << endl;
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("ports:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (this->ports.empty() && extractPortValue(kv)) {
					for (size_t i = 0; i < this->ports.size(); i++)
						cout << "ports[" << i << "]: " << "'" << this->ports[i] << "'" << ",";
					cout << endl;
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("server_names:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (this->serverNames.empty() && extractServerNamesValue(kv)) {
					for (size_t i = 0; i < this->serverNames.size(); i++)
						cout << "server name[" << i << "]: " << "'" << this->serverNames[i] << "'" << endl;;
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("error_pages:") != string::npos) {
				if (!extractErrorPages(file, currentLineNumber)) syntaxError(currentLineNumber);
				cout << "Line li kantsnah yrje3 ah: " << line << endl;
				for (map<int, string>::iterator it = this->errorPages.begin(); it != this->errorPages.end(); it++)
					cout << "errorPage[" << it->first << "] = " << it->second << endl;
			}
			else if (line.find("client_max_body_size:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (this->maxBodySize == -1 && extractClientMaxBodySizeValue(kv)) {
					cout << "client max body size: " << this->maxBodySize << endl;	
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("location:") != string::npos){
				cout << "L7ma9 ya zebi daba" << endl;
			}
			else syntaxError(currentLineNumber);
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

bool extractLocationInfos(ifstream& file, int& currentLineNumber) {
	
}


bool ConfigurationParser::extractErrorPages(ifstream& file, int& currentLineNumber) {
	string line;
	bool foundValidErrorPage = false;

	while (getline(file, line) && ++currentLineNumber) {
		if (LineIsCommentOrEmpty(line)) continue ;

		int CurrentIndentLevel = getIndentLevel(line);
		if (CurrentIndentLevel != 2) {
			/* Move back one line */
			cout << "line wesst rj3a: " << line << endl;
			cout << "current line num fl error != 2" << currentLineNumber <<endl;
			file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
			currentLineNumber--;
			/* return false if we haven't found at least we valid error page */
			return foundValidErrorPage;
		}
		clear_kv(kv);
		if (!verifyLineFormat(line, 1))
			return false;
		for (size_t i = 0; i < kv.key.length(); i++) {
			if (!isdigit(kv.key[i])) return false;
		}
		int value = 0;
		istringstream iss(kv.key);
		iss >> value;
		if (value < 400 || value > 599) return false;

		this->errorPages[value] = kv.value;
		cout << "safy ha7na storina asadi9" << endl;
		foundValidErrorPage = true;
	}
	return foundValidErrorPage;
}

bool ConfigurationParser::extractClientMaxBodySizeValue(key_value& k_v) {
	if (k_v.key != "client_max_body_size") return false;
	if (k_v.value.empty()) return false;
	
	for (size_t i = 0; i < k_v.value.length(); i++) {
		if (!isdigit(k_v.value[i])) return false;
	}
	ssize_t value = 0;
	istringstream iss(k_v.value);
	iss >> value;
	
	if (value < 0) return false;
	if (value > MAX_BODY_SIZE) return false;

	this->maxBodySize = value;
	return true;	
}

bool ConfigurationParser::extractServerNamesValue(key_value& k_v) {
	if (k_v.key != "server_names") return false;
	if (k_v.value.empty()) return false;
	string parsed, input = k_v.value;
	stringstream input_ss(input);

	while (getline(input_ss, parsed, ' ')) {
		if (parsed.empty()) return false;
		this->serverNames.push_back(parsed);
	}
	return true;
}

bool ConfigurationParser::isValidPortSegment(const string& segment) {
	if (kv.key != "ports") return false;
	if (segment.empty()) return false;

	for (size_t i = 0; i < segment.length(); i++) {
		if (!isdigit(segment[i])) return false;
	}

	int value = 0;
	istringstream iss(segment);
	iss >> value;

	return (value >= 1024 && value <= 65535);
}

bool ConfigurationParser::extractPortValue(key_value& k_v) {
	/* 1024 -> 65535 : Registred ports. */
	string parsed, input = k_v.value;
	stringstream input_ss(input);
	
	while (getline(input_ss, parsed, ',')) {
		if (!isValidPortSegment(parsed)) return false;
		this->ports.push_back(parsed);
	}
	return true;
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

bool ConfigurationParser::extractHostKey(key_value& k_v) { 
	int currentCountOfSubnets = 0;
	string parsed, input = k_v.value;
	stringstream input_ss(input);
	while (getline(input_ss, parsed, '.') && ++currentCountOfSubnets) {
		if (!isValidIPSegment(parsed)) return false;
		if (currentCountOfSubnets > 4) return false;
	}
	k_v.currentParsedValue = k_v.value;
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
	cout << "ColonCount: " << colonCount << endl;
	if (colonCount != 1) return false;

	/* na5do pos of colona */
	size_t colonPosition = line.find(':');
	/* nshofo l key */
	kv.key = line.substr(0, colonPosition);
	if (kv.key.empty() || kv.key.find(' ') != string::npos || kv.key.find('\t') != string::npos)
		return false;
	cout << "\nkey: " << kv.key << endl;
	//if (kv.key == "error_pages") return true;
	/* daba value dial dak key */
	kv.value = line.substr(colonPosition+1);
	size_t firstNoSpace = kv.value.find_first_not_of(" \t");
	size_t lastNoSpace = kv.value.find_last_not_of(" \t");
	if ((kv.value.empty() || firstNoSpace == string::npos) && kv.key == "error_pages") return true;
	if (kv.value.empty() || firstNoSpace == string::npos) return false;
	if (firstNoSpace != string::npos && lastNoSpace  != string::npos)
		kv.value = kv.value.substr(firstNoSpace, lastNoSpace - firstNoSpace + 1);
	cout << "value: " << kv.value<< "\n" << endl;
	return true;
}

void ConfigurationParser::syntaxError(int currentLineNumber) {
	ostringstream oss;
	oss << currentLineNumber;
	throw runtime_error("error: syntax error at line " + oss.str());
}

void ConfigurationParser::clear_kv(key_value& kv) {
	/*cout << endl;
	cout << "before cleaning..." << endl;
	cout << "kv.value: " << kv.value << ", kv.key: " << kv.key << ", kv.currentParsedValue: " << kv.currentParsedValue << endl;*/
	kv.key.clear();
	kv.value.clear();
	kv.currentParsedValue.clear();
	/* cout << endl;
	cout << "Clearing..." << endl;
	cout << "kv.value: " << kv.value << ", kv.key: " << kv.key << ", kv.currentParsedValue: " << kv.currentParsedValue << endl; */
}
