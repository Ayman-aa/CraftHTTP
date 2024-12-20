/* -- main.cpp -- */

# include <iostream>
# include <cstdio>
# include "config/configParser.hpp"


/* global var */
const char *filePath;
char *error;


const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";

void printAllServers(const vector<ServerConfiguration*>& servers) {
    cout << "\n" << MAGENTA << "=====================================";
    cout << "\n||      WEBSERV CONFIGURATION      ||";
    cout << "\n=====================================" << RESET << "\n\n";
    
    if (servers.empty()) {
        cout << RED << "No servers configured!" << RESET << endl;
        return;
    }

    for (size_t i = 0; i < servers.size(); ++i) {
        cout << CYAN << "\n┌──────────────────────────────┐" << endl;
        cout << "│      SERVER BLOCK " << (i + 1) << "          │" << endl;
        cout << "└──────────────────────────────┘" << RESET << "\n\n";

        if (servers[i] == NULL) {
            cout << RED << "Error: Invalid server configuration at index " << i << RESET << endl;
            continue;
        }

        // Print Server Basic Info
        cout << YELLOW << "Basic Configuration:" << RESET << endl;
        cout << "├─ Host: " << servers[i]->host << endl;
        
        // Print Ports
        cout << "├─ Ports: ";
        for (size_t j = 0; j < servers[i]->ports.size(); ++j) {
            cout << servers[i]->ports[j];
            if (j < servers[i]->ports.size() - 1) cout << ", ";
        }
        cout << endl;

        // Print Server Names
        cout << "├─ Server Names: ";
        if (servers[i]->serverNames.empty()) {
            cout << "default";
        } else {
            for (size_t j = 0; j < servers[i]->serverNames.size(); ++j) {
                cout << servers[i]->serverNames[j];
                if (j < servers[i]->serverNames.size() - 1) cout << ", ";
            }
        }
        cout << endl;

        // Print Body Size Limits
        cout << "├─ Max Body Size: " << 
            (servers[i]->maxBodySize == -1 ? "unlimited" : to_string(servers[i]->maxBodySize) + " bytes") 
            << endl;

        // Print Error Pages
        if (!servers[i]->errorPages.empty()) {
            cout << "\n" << GREEN << "Error Pages:" << RESET << endl;
            for (map<int, string>::const_iterator it = servers[i]->errorPages.begin(); 
                 it != servers[i]->errorPages.end(); ++it) {
                cout << "├─ " << it->first << ": " << it->second << endl;
            }
        }

        // Print Locations
        if (!servers[i]->locations.empty()) {
            cout << "\n" << BLUE << "Location Blocks:" << RESET << endl;
            for (map<string, Location>::const_iterator locIt = servers[i]->locations.begin(); 
                 locIt != servers[i]->locations.end(); ++locIt) {
                cout << "\n┌─ Path: " << locIt->first << endl;
                
                // Print Location Details
                if (!locIt->second.root.empty())
                    cout << "├─ Root: " << locIt->second.root << endl;
                
                cout << "├─ Autoindex: " << (locIt->second.autoindex ? "on" : "off") << endl;
                
                if (!locIt->second.allow_methods.empty()) {
                    cout << "├─ Allowed Methods: ";
                    for (size_t j = 0; j < locIt->second.allow_methods.size(); ++j) {
                        cout << locIt->second.allow_methods[j];
                        if (j < locIt->second.allow_methods.size() - 1) cout << ", ";
                    }
                    cout << endl;
                }

                if (!locIt->second.index.empty()) {
                    cout << "├─ Index Files: ";
                    for (size_t j = 0; j < locIt->second.index.size(); ++j) {
                        cout << locIt->second.index[j];
                        if (j < locIt->second.index.size() - 1) cout << ", ";
                    }
                    cout << endl;
                }

                if (!locIt->second.redirection_return.empty())
                    cout << "├─ Redirection: " << locIt->second.redirection_return << endl;

                if (!locIt->second.cgi_path.empty()) {
                    cout << "├─ CGI Configuration:" << endl;
                    for (map<string, string>::const_iterator cgiIt = locIt->second.cgi_path.begin(); 
                         cgiIt != locIt->second.cgi_path.end(); ++cgiIt) {
                        cout << "│  ├─ " << cgiIt->first << ": " << cgiIt->second << endl;
                    }
                }
                cout << "└────────────────────" << endl;
            }
        }
        
        cout << "\n" << CYAN << "=====================================\n" << RESET;
    }
}



























int main(int ac, char *av[])
{
	if (ac != 2) {
		if (ac > 2) {
			fprintf(stderr, "[Error] Invalid number of arguments. Usage: %s <config_file>\n", av[0]);
			return 1;
		}
		fprintf(stderr, "[Warning] No configuration file provided. Proceeding with default configuration.\n");
		filePath = "conf/test.yaml";
	}
	else
		filePath = av[1];
	fprintf(stdout, "filePath: %s\n", filePath);
	string fp = filePath;

	try {
		ConfigurationParser fileParser(fp);
		printAllServers(&fileParser.servers)
		//printAllLocations(fileParser);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
	return 0;
}
