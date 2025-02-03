/* -- main.cpp -- */

#include "./includes/configParser.hpp"
#include "./includes/cluster.hpp"
#include "./includes/server.hpp"

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

// Helper function to convert int to string in C++98
std::string intToString(int num) {
    std::stringstream ss;
    ss << num;
    return ss.str();
}

void printAllServers(const vector<ServerConfiguration*>& servers) {
    cout << "\n" << MAGENTA << "=====================================";
    cout << "\n||      WEBSERV WITH " << servers.size() << " SERVS       ||";
    cout << "\n=============================.=======" << RESET << "\n\n";
    
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

        // Print Body Size Limits - Modified this line
        cout << "├─ Max Body Size: " << 
            (servers[i]->maxBodySize == -1 ? "unlimited" : intToString(servers[i]->maxBodySize) + " bytes") 
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

                if (!locIt->second.root.empty())
                    cout << "├─ Upload_path: " << locIt->second.upload_path << endl;
                
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





int main(int ac, char *av[]) {
    std::string filePath;

    if (ac != 2) {
        if (ac > 2) {
            std::cerr << RED << "[!] Too many args. Usage: " << av[0] << " <config>" << RESET << std::endl;
            return 1;
        }
        std::cerr << YELLOW << "[!] No config. Using default." << RESET << std::endl;
        filePath = "conf/test.yaml";
    } else
        filePath = av[1];
 

    std::cout << CYAN << "[~] Config: " << filePath << RESET << std::endl;

    try {
        // Assuming ConfigurationParser and printAllServers are defined elsewhere
        ConfigurationParser fileParser(filePath);
        printAllServers(fileParser.servers);
        // printAllLocations(fileParser);
    } catch (const std::exception &e) {
        std::cerr << RED << "[X] Error: " << e.what() << RESET << std::endl;
        return 1;
    }

    return 0;
}
