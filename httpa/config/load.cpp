/* -- laod.cpp -- */

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
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (!servLocationLine(kv)) syntaxError(currentLineNumber);
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

