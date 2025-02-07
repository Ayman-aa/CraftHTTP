/* -- laod.cpp -- */

#include "../includes/configParser.hpp"

key_value kv;
/*
 * Opens and parses a webserv config file
 * throw exception if !file.isopen()
 * and call the main parsing method [LOAD()]
 */
ConfigurationParser::ConfigurationParser(string& filePath) {
	ifstream file(filePath.c_str());
	if (!file.is_open())
		throw runtime_error("failed to open file: " + filePath);
	ConfigurationParser::load(file);
	file.close();
}

/*
 * Main parsing method
 * Reads through config file and create server configurations
 */
void ConfigurationParser::load(ifstream& file) {
	string line;
	int currentLineNumber = 0;

	while (getline(file, line) && ++currentLineNumber) {
		if (LineIsCommentOrEmpty(line)) continue;
		if (!isValidRootLevel(line)) syntaxError(currentLineNumber, SERVER_ERROR);

		/* Create new server for each server block */

		while (getline(file, line) && ++currentLineNumber) {
			int currentIndentLevel = getIndentLevel(line);

			clear_kv(kv);

			if (LineIsCommentOrEmpty(line)) continue ;

			if (line == "server:"  && !currentIndentLevel ) {
				servers.push_back(new ServerConfiguration(currentServer));
				currentServer = ServerConfiguration();
				FileSeekg(file, line, currentLineNumber, true);
				break;
			}
			if (currentIndentLevel != 1) syntaxError(currentLineNumber, SYNTAX_ERROR);

			CHECK_AND_EXTRACT("host:", currentServer.host, extractHostKey);
			CHECK_AND_EXTRACT("ports:", currentServer.ports, extractPortValue);
			CHECK_AND_EXTRACT("server_names:", currentServer.serverNames, extractServerNamesValue);
			CHECK_AND_EXTRACT_MAX_BODY_SIZE("client_max_body_size:", currentServer.maxBodySize, extractClientMaxBodySizeValue);

			if (line.find("error_pages:") != string::npos) {
				if (!extractErrorPages(file, currentLineNumber)) syntaxError(currentLineNumber, SYNTAX_ERROR);
				continue;
			}
			if (line.find("location:") != string::npos){
				HANDLE_LOCATION();
				continue;
			}
			else syntaxError(currentLineNumber, SYNTAX_ERROR); 
		}
	}
	servers.push_back(new ServerConfiguration(currentServer));
}

/* ash ba4i t3ref ??? */
bool ConfigurationParser::isValidRootLevel(string& line) {
	int rootIndentLevel = getIndentLevel(line);
		return rootIndentLevel == 0 && line == "server:";
}

/* ash ba4i t3ref ??? */
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

bool ConfigurationParser::FileSeekg(ifstream& file, const string& line, int &currentLineNumber, bool retVal) {
	file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
	currentLineNumber--;
	return retVal;
}
