#ifndef SERVERCONFIGURATION_HPP
#define SERVERCONFIGURATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;
class ServerConfiguration
{
    public:
        int port;
        string host;
        string bodySize;
        size_t maxBodySize;
        vector<string> names;
        map<string, string> errorPages;
        map<string, string> locations;
        
};

#endif