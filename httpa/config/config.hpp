/* -- config.hpp -- */

#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cctype>

using namespace std;

struct Location
{
    // Location parameters
    bool autoindex;
    string root;
    string path; /* hada */
    string redirection_return;
    string upload_path; /* ou hada */
    vector<string> allow_methods;
    vector<string> index;
    map<string, string> cgi_path;  /* ou hadi baynin */
};

class ServerConfiguration
{
    public:
        // Constructor
        // parameters from server block
        vector<string> ports;
        string host;
        string bodySize;
        ssize_t maxBodySize;
        vector<string> serverNames;
        map<int, string> errorPages;
        map<string, Location> locations;
        // Getters
        string getErrorPage(string code);
        string getLocations(string path);
        ServerConfiguration getServerConfiguration(const string &host, string &port, string &ip);
};

#endif /* ayeh ayeh, config.hpp */
