#ifndef SERVERCONFIGURATION_HPP
#define SERVERCONFIGURATION_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
using namespace std;

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
class ServerConfiguration
{
    public:
        string port;
        string host;
        string bodySize;
        size_t maxBodySize;
        vector<string> names;
        map<string, string> errorPages;
        map<string, Location> locations;
};

#endif