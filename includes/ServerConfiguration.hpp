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
        map<string, Location> locations;
        
};

struct Location
{
    bool autoindex = false;
    string root;
    string path;
    string redirection_return;
    string upload_path;
    vector<string> allow_methods;
    vector<string> index;
    map<string, string> cgi;
};
#endif