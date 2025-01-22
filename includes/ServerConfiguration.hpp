#ifndef SERVERCONFIGURATION_HPP
#define SERVERCONFIGURATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>

using namespace std;

struct Location
{
    // Location parameters
    bool autoindex;
    string root;
    string path;
    string redirection_return;
    string upload_path;
    vector<string> allow_methods;
    vector<string> index;
    map<string, string> cgi_path;
	bool thereIsMethod(std::string &method);
	bool thereIsCGI(std::string &cgi);
};

class ServerConfiguration
{
    public:
		// Default Constructor
		ServerConfiguration();

		// Copy Constructor
		ServerConfiguration(ServerConfiguration* other);

        // parameters from server block
        vector<string> ports;
        string host;
        string bodySize;
        ssize_t maxBodySize;
        vector<string> serverNames;
        map<int, string> errorPages;
        map<string, Location> locations;
        bool hasLocation(std::string &location);
        Location &getLocation(std::string &location);
};

#endif