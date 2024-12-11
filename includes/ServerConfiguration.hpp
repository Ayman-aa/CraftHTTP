#ifndef SERVERCONFIGURATION_HPP
#define SERVERCONFIGURATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <unistd.h>
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
};

class ServerConfiguration
{
    public:
        // Constructor
        ServerConfiguration(){};
        ServerConfiguration(string configFile);
        // parameters from server block
        vector<string> ports;
        string host;
        string bodySize;
        size_t maxBodySize;
        vector<string> serverNames;
        map<string, string> errorPages;
        map<string, Location> locations;
        // Getters
        string getErrorPage(string code);
        string getLocations(string path);
        ServerConfiguration getServerConfiguration(const string &host, string &port, string &ip);
};

#endif