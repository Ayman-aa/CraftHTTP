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
				/* Move file pointer back to reprocess the "server:" line */
				FileSeekg(file, line, currentLineNumber);
				break;
			}
			if (currentIndentLevel != 1) syntaxError(currentLineNumber, SYNTAX_ERROR);

			if (line.find("host:") != string::npos) {
				if (!verifyLineFormat(line, currentIndentLevel)) syntaxError(currentLineNumber, SYNTAX_ERROR);
				if (extractHostKey(kv))  {
					if (!currentServer.host.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
					currentServer.host = kv.currentParsedValue;
				}
				else syntaxError(currentLineNumber, SYNTAX_ERROR);
			}
			else if (line.find("ports:") != string::npos) {
				if (!verifyLineFormat(line, currentIndentLevel)) syntaxError(currentLineNumber, SYNTAX_ERROR);
				if (!currentServer.ports.empty())  syntaxError(currentLineNumber, DUPLICATE_ENTRY);
				if (!extractPortValue(kv)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			}
			else if (line.find("server_names:") != string::npos) {
				if (!verifyLineFormat(line, currentIndentLevel)) syntaxError(currentLineNumber, SYNTAX_ERROR);
				if (!currentServer.serverNames.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
				if (!extractServerNamesValue(kv)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			}
			else if (line.find("error_pages:") != string::npos) {
				if (!extractErrorPages(file, currentLineNumber)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			}
			else if (line.find("client_max_body_size:") != string::npos) {
				if (!verifyLineFormat(line, currentIndentLevel)) syntaxError(currentLineNumber, SYNTAX_ERROR);
				if (!(currentServer.maxBodySize == -1)) syntaxError(currentLineNumber, DUPLICATE_ENTRY);
				if (!extractClientMaxBodySizeValue(kv)) syntaxError(currentLineNumber, SYNTAX_ERROR);
			}
			else if (line.find("location:") != string::npos){
				Location location; /* reseti l9lawi nchoufou */
				if (!verifyLineFormat(line, currentIndentLevel)) syntaxError(currentLineNumber, SYNTAX_ERROR);
				if (!servLocationLine(kv, location)) syntaxError(currentLineNumber, LOCATION_ERROR );
				if (!extractLocationInfos(file, currentLineNumber, location)) syntaxError(currentLineNumber, SERVER_BLOCK_ERROR);
				currentServer.locations[location.path] = location;
			}
			else syntaxError(currentLineNumber, SYNTAX_ERROR); 
		}
	}
	servers.push_back(new ServerConfiguration(currentServer));
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

void ConfigurationParser::FileSeekg(ifstream& file, const string& line, int &currentLineNumber) {
	file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
	currentLineNumber--;
}