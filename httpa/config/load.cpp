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
	bool firstServer = true;

	while (getline(file, line) && ++currentLineNumber) {
		cout << "OUTER LOOP - Current line: '" << line << "'" << endl;
		if (!isValidRootLevel(line)) syntaxError(currentLineNumber);

		/* Create new server for each server block */
		/*if (!firstServer)
			servers.push_back(new ServerConfiguration(currentServer));
		firstServer = false; */

		while (getline(file, line) && ++currentLineNumber) {
			int currentIndentLevel = getIndentLevel(line);
			clear_kv(kv);
			cout << "INNER LOOP - Current line: '" << line << "'" << endl;
			cout << "Current indent level: " << currentIndentLevel << endl;
			if (LineIsCommentOrEmpty(line)) continue ;
			if (line == "server:"  && !currentIndentLevel ) {
				cout << "Found new server block, pushing current server and breaking" << endl;
				servers.push_back(new ServerConfiguration(currentServer));
				currentServer = ServerConfiguration();
				file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
				currentLineNumber--;
				break;
			}
			if (currentIndentLevel != 1) syntaxError(currentLineNumber);

			if (line.find("host:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (extractHostKey(kv))  {
					if (!currentServer.host.empty()) syntaxError(currentLineNumber);
					currentServer.host = kv.currentParsedValue;
					cout << "currentServer.host: " << currentServer.host << endl;
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("ports:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (currentServer.ports.empty() && extractPortValue(kv)) {
					for (size_t i = 0; i < currentServer.ports.size(); i++)
						cout << "ports[" << i << "]: " << "'" << currentServer.ports[i] << "'" << ",";
					cout << endl;
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("server_names:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (currentServer.serverNames.empty() && extractServerNamesValue(kv)) {
					for (size_t i = 0; i < currentServer.serverNames.size(); i++)
						cout << "server name[" << i << "]: " << "'" << currentServer.serverNames[i] << "'" << endl;;
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("error_pages:") != string::npos) {
				if (!extractErrorPages(file, currentLineNumber)) syntaxError(currentLineNumber);
				cout << "Line li kantsnah yrje3 ah: " << line << endl;
				for (map<int, string>::iterator it = currentServer.errorPages.begin(); it != currentServer.errorPages.end(); it++)
					cout << "errorPage[" << it->first << "] = " << it->second << endl;
			}
			else if (line.find("client_max_body_size:") != string::npos) {
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (currentServer.maxBodySize == -1 && extractClientMaxBodySizeValue(kv)) {
					cout << "client max body size: " << currentServer.maxBodySize << endl;	
				}
				else syntaxError(currentLineNumber);
			}
			else if (line.find("location:") != string::npos){
				Location location; /* reseti l9lawi nchoufou */
				cout << "tmi7wa lwla, lal9itini mrra a5ra y3ni rah slet weld nass" << endl;
				(!verifyLineFormat(line, currentIndentLevel) ? syntaxError(currentLineNumber): (void)0);
				if (!servLocationLine(kv, location)) syntaxError(currentLineNumber);
				if (!extractLocationInfos(file, currentLineNumber, location)) syntaxError(currentLineNumber);
				currentServer.locations[location.path] = location;
				cout << "Hanhna 5rejna ya zebi" << endl;
			}
			else
			{
				cout << "l3assba, that's it" << endl;
			 	syntaxError(currentLineNumber); 
			}
		}
		if (!firstServer)
			servers.push_back(new ServerConfiguration(currentServer));
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

