/* -- laod.cpp -- */

#include "ConfigurationParser.hpp"

key_value kv;

ConfigurationParser::ConfigurationParser(string& filePath) {
	ifstream file(filePath.c_str());
	if (!file.is_open())
		throw runtime_error("failed to open file: " + filePath);
	ConfigurationParser::load(file);
	file.close();
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

bool ConfigurationParser::FileSeekg(ifstream& file, const string& line, int &currentLineNumber, bool retVal) {
	file.seekg(file.tellg() - static_cast<streamoff>(line.length() + 1));
	currentLineNumber--;
	return retVal;
}
ServerConfiguration* ConfigurationParser::getServerConfig(const std::string& ip, const std::vector<std::string>& ports, const std::string& host) {
    std::vector<ServerConfiguration*> tmp;

    std::cout << "Searching for server config with IP: " << ip << ", ports: ";
    for (size_t i = 0; i < ports.size(); ++i) {
        std::cout << ports[i] << " ";
    }
    std::cout << ", host: " << host << std::endl;

    for (std::vector<ServerConfiguration*>::iterator it = servers.begin(); it != servers.end(); it++) {
        if (*it == nullptr) {
            std::cerr << "Error: Null ServerConfiguration pointer found" << std::endl;
            continue;
        }

        std::cout << "Checking server config with IP: " << (*it)->host << ", ports: ";
        for (size_t i = 0; i < (*it)->ports.size(); ++i) {
            std::cout << (*it)->ports[i] << " ";
        }
        std::cout << ", server names: ";
        for (size_t i = 0; i < (*it)->serverNames.size(); ++i) {
            std::cout << (*it)->serverNames[i] << " ";
        }
        std::cout << std::endl;

        if (ip == (*it)->host) {
            for (size_t i = 0; i < ports.size(); ++i) {
                if (std::find((*it)->ports.begin(), (*it)->ports.end(), ports[i]) != (*it)->ports.end()) {
                    if (std::find((*it)->serverNames.begin(), (*it)->serverNames.end(), host) != (*it)->serverNames.end())
                        return *it;
                    if (tmp.empty())
                        tmp.push_back(*it);
                }
            }
        }
    }
    if (!tmp.empty())
        return tmp[0];
    throw std::runtime_error("Server not found");
}