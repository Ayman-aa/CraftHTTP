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

void printLocation(const Location& location, const string& path = "") {
    std::cout << BLUE << "  Location Details" << (path.empty() ? "" : ": " + path) << RESET << std::endl;
    
    std::cout << "    " << YELLOW << "Autoindex: " << RESET 
              << (location.autoindex ? "On" : "Off") << std::endl;
    
    if (!location.root.empty()) {
        std::cout << "    " << YELLOW << "Root: " << RESET 
                  << location.root << std::endl;
    }
    
    if (!location.redirection_return.empty()) {
        std::cout << "    " << RED << "Redirection: " << RESET 
                  << location.redirection_return << std::endl;
    }
    
    if (!location.upload_path.empty()) {
        std::cout << "    " << YELLOW << "Upload Path: " << RESET 
                  << location.upload_path << std::endl;
    }
    
    // Allowed Methods
    if (!location.allow_methods.empty()) {
        std::cout << "    " << CYAN << "Allowed Methods: " << RESET;
        for (size_t i = 0; i < location.allow_methods.size(); ++i) {
            std::cout << location.allow_methods[i];
            if (i < location.allow_methods.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    // Index Files
    if (!location.index.empty()) {
        std::cout << "    " << GREEN << "Index Files: " << RESET;
        for (size_t i = 0; i < location.index.size(); ++i) {
            std::cout << location.index[i];
            if (i < location.index.size() - 1) std::cout << ", ";
        }
        std::cout << std::endl;
    }
    
    // CGI Paths
    if (!location.cgi_path.empty()) {
        std::cout << "    " << MAGENTA << "CGI Paths:" << RESET << std::endl;
        for (map<string, string>::const_iterator 
             cgiIt = location.cgi_path.begin(); 
             cgiIt != location.cgi_path.end(); ++cgiIt) {
            std::cout << "      " << cgiIt->first << ": " 
                      << cgiIt->second << std::endl;
        }
    }
}

void printServerConfiguration(const ServerConfiguration& config) {
    std::cout << RED << "===== Server Configuration =====" << RESET << std::endl;
    
    // Ports
    std::cout << YELLOW << "Ports: " << RESET;
    for (size_t i = 0; i < config.ports.size(); ++i) {
        std::cout << config.ports[i];
        if (i < config.ports.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    // Host
    std::cout << YELLOW << "Host: " << RESET << config.host << std::endl;

    // Body Size
    std::cout << YELLOW << "Body Size: " << RESET 
              << config.bodySize 
              << " (Max: " << config.maxBodySize << " bytes)" << std::endl;

    // Server Names
    std::cout << YELLOW << "Server Names: " << RESET;
    for (size_t i = 0; i < config.serverNames.size(); ++i) {
        std::cout << config.serverNames[i];
        if (i < config.serverNames.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    // Error Pages
    std::cout << GREEN << "Error Pages:" << RESET << std::endl;
    for (map<int, string>::const_iterator it = config.errorPages.begin(); 
         it != config.errorPages.end(); ++it) {
        std::cout << "  " << it->first << ": " << it->second << std::endl;
    }

    // Locations
    std::cout << BLUE << "Locations:" << RESET << std::endl;
    for (map<string, Location>::const_iterator locIt = config.locations.begin(); 
         locIt != config.locations.end(); ++locIt) {
        printLocation(locIt->second, locIt->first);
        std::cout << std::endl;
    }

    std::cout << RED << "===== End of Server Configuration =====" << RESET << std::endl;
}

// Function to print a specific location
void printSpecificLocation(const ServerConfiguration& config, const string& locationPath) {
    map<string, Location>::const_iterator locIt = config.locations.find(locationPath);
    
    if (locIt != config.locations.end()) {
        std::cout << RED << "===== Location Details =====" << RESET << std::endl;
        printLocation(locIt->second, locIt->first);
        std::cout << RED << "===== End of Location Details =====" << RESET << std::endl;
    } else {
        std::cout << RED << "Location path not found: " << locationPath << RESET << std::endl;
    }
}
void printAllLocations(const ServerConfiguration& config) {
    std::cout << BLUE << "===== All Locations =====" << RESET << std::endl;
    
    // Check if locations map is empty
    if (config.locations.empty()) {
        std::cout << YELLOW << "No locations configured." << RESET << std::endl;
        return;
    }

    // Iterate through all locations
    for (map<string, Location>::const_iterator locIt = config.locations.begin(); 
         locIt != config.locations.end(); ++locIt) {
        // Print the location path
        std::cout << CYAN << "Location Path: " << locIt->first << RESET << std::endl;
        
        // Print location details
        printLocation(locIt->second);
        
        // Add a separator between locations
        std::cout << std::string(50, '-') << std::endl;
    }

    std::cout << BLUE << "===== End of Locations =====" << RESET << std::endl;
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
		printServerConfiguration(fileParser);
		//printAllLocations(fileParser);
	}
	catch (exception& e) {
		cout << e.what() << endl;
	}
	return 0;
}
