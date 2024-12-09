#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>

class YAMLParser {
private:
    struct Location {
        std::vector<std::string> allow_methods;
        std::string root;
        bool autoindex;
        std::vector<std::string> index;
        std::map<std::string, std::string> cgi_path;
        std::string upload_path;
        std::string return_url;
        
        Location() : autoindex(false) {}
    };

    struct Server {
        std::string listen;
        long long max_client_body_size;
        std::string server_names;
        std::map<int, std::string> error_pages;
        std::map<std::string, Location> locations;
        
        Server() : max_client_body_size(0) {}
    };

    std::vector<Server> servers;
    
    int getCurrentIndentation(const std::string& line) {
        size_t spaces = 0;
        while (spaces < line.length() && line[spaces] == ' ') {
            spaces++;
        }
        return spaces;
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t");
        return str.substr(first, last - first + 1);
    }
    
    void parseLine(const std::string& line, std::string& key, std::string& value) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            key = trim(line.substr(0, colonPos));
            value = trim(line.substr(colonPos + 1));
        }
    }

    bool isListItem(const std::string& line) {
        return trim(line)[0] == '-';
    }

    std::string getListItemContent(const std::string& line) {
        size_t dashPos = line.find('-');
        if (dashPos == std::string::npos) return "";
        return trim(line.substr(dashPos + 1));
    }

public:
    void parse(const std::string& filename) {
        std::ifstream file(filename.c_str());
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return;
        }

        std::string line;
        Server* currentServer = NULL;
        Location* currentLocation = NULL;
        std::string currentSection;
        std::string currentSubSection;
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::string trimmedLine = trim(line);
            if (trimmedLine.empty()) continue;

            int indent = getCurrentIndentation(line);
            std::string key, value;
            
            if (isListItem(trimmedLine)) {
                std::string content = getListItemContent(trimmedLine);
                if (currentSection == "error_page" && currentServer) {
                    // Parse error pages
                    size_t colonPos = content.find(':');
                    if (colonPos != std::string::npos) {
                        int errorCode;
                        std::istringstream(content.substr(0, colonPos)) >> errorCode;
                        currentServer->error_pages[errorCode] = trim(content.substr(colonPos + 1));
                    }
                }
                else if (currentSection == "allow_methods" && currentLocation) {
                    currentLocation->allow_methods.push_back(content);
                }
                else if (currentSection == "index" && currentLocation) {
                    currentLocation->index.push_back(content);
                }
                else if (currentSection == "cgi_path" && currentLocation) {
                    size_t colonPos = content.find(':');
                    if (colonPos != std::string::npos) {
                        std::string type = trim(content.substr(0, colonPos));
                        std::string path = trim(content.substr(colonPos + 1));
                        currentLocation->cgi_path[type] = path;
                    }
                }
                continue;
            }

            parseLine(trimmedLine, key, value);

            if (indent == 0 && key == "server") {
                servers.push_back(Server());
                currentServer = &servers.back();
                currentLocation = NULL;
                currentSection = "";
                currentSubSection = "";
            }
            else if (currentServer) {
                if (indent == 2) {
                    currentSection = key;
                    if (key == "listen") {
                        currentServer->listen = value;
                    }
                    else if (key == "max_client_body_size") {
                        std::istringstream iss(value);
                        iss >> currentServer->max_client_body_size;
                    }
                    else if (key == "server_names") {
                        currentServer->server_names = value;
                    }
                    else if (key == "location") {
                        currentLocation = &(currentServer->locations[value]);
                    }
                }
                else if (indent == 4 && currentLocation) {
                    currentSection = key;
                    if (key == "root") {
                        currentLocation->root = value;
                    }
                    else if (key == "autoindex") {
                        currentLocation->autoindex = (value == "on");
                    }
                    else if (key == "return") {
                        currentLocation->return_url = value;
                    }
                    else if (key == "upload_path") {
                        currentLocation->upload_path = value;
                    }
                }
            }
        }
        file.close();
    }

    void printConfig() {
        for (size_t i = 0; i < servers.size(); ++i) {
            std::cout << "Server " << i + 1 << ":\n";
            std::cout << "  Listen: " << servers[i].listen << "\n";
            std::cout << "  Server Names: " << servers[i].server_names << "\n";
            if (servers[i].max_client_body_size > 0) {
                std::cout << "  Max Client Body Size: " << servers[i].max_client_body_size << "\n";
            }
            
            if (!servers[i].error_pages.empty()) {
                std::cout << "  Error Pages:\n";
                std::map<int, std::string>::const_iterator errIt;
                for (errIt = servers[i].error_pages.begin(); errIt != servers[i].error_pages.end(); ++errIt) {
                    std::cout << "    - " << errIt->first << ": " << errIt->second << "\n";
                }
            }
            
            std::cout << "  Locations:\n";
            std::map<std::string, Location>::const_iterator locIt;
            for (locIt = servers[i].locations.begin(); locIt != servers[i].locations.end(); ++locIt) {
                std::cout << "    " << locIt->first << ":\n";
                std::cout << "      Root: " << locIt->second.root << "\n";
                std::cout << "      Autoindex: " << (locIt->second.autoindex ? "on" : "off") << "\n";
                
                if (!locIt->second.allow_methods.empty()) {
                    std::cout << "      Allow Methods:\n";
                    for (size_t j = 0; j < locIt->second.allow_methods.size(); ++j) {
                        std::cout << "        - " << locIt->second.allow_methods[j] << "\n";
                    }
                }
                
                if (!locIt->second.index.empty()) {
                    std::cout << "      Index:\n";
                    for (size_t j = 0; j < locIt->second.index.size(); ++j) {
                        std::cout << "        - " << locIt->second.index[j] << "\n";
                    }
                }
                
                if (!locIt->second.cgi_path.empty()) {
                    std::cout << "      CGI Paths:\n";
                    std::map<std::string, std::string>::const_iterator cgiIt;
                    for (cgiIt = locIt->second.cgi_path.begin(); cgiIt != locIt->second.cgi_path.end(); ++cgiIt) {
                        std::cout << "        - " << cgiIt->first << ": " << cgiIt->second << "\n";
                    }
                }
                
                if (!locIt->second.return_url.empty()) {
                    std::cout << "      Return: " << locIt->second.return_url << "\n";
                }
                
                if (!locIt->second.upload_path.empty()) {
                    std::cout << "      Upload Path: " << locIt->second.upload_path << "\n";
                }
            }
            std::cout << "\n";
        }
    }
};

int main() {
    YAMLParser parser;
    parser.parse("config.yaml");
    parser.printConfig();
    return 0;
}
