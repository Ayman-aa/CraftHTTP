/* -- config.hpp -- */

#pragma once

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "includes.hpp"
#define CHECK_DUPLICATE(FIELD) \
	if (!FIELD.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY) \

#define CHECK_DUPLICATE_STR(FIELD) \
	if (FIELD != "") syntaxError(currentLineNumber, DUPLICATE_ENTRY) \

#define CHECK_AND_EXTRACT_STR(FIELD, CONTAINER, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		CHECK_DUPLICATE_STR(CONTAINER); \
		if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		findLoc = true; \
		continue ; \
	} \

#define CHECK_AND_EXTRACT_CONTAINER(FIELD, CONTAINER, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		CHECK_DUPLICATE(CONTAINER); \
		if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		findLoc = true; \
		continue ; \
	} \

#define CHECK_AND_EXTRACT_BOOL(FIELD, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		findLoc = true; \
		continue ; \
	} \


struct Location
{
    bool autoindex;
    string root;
    string path;
    string redirection_return;
    string upload_path;
    vector<string> allow_methods;
    vector<string> index;
    map<string, string> cgi_path;
    
    /* WHO WHA */
    Location() : autoindex(false) {}
};

class ServerConfiguration
{
    public:
        /* WHO HA*/
		ServerConfiguration(): maxBodySize(-1) {}

        /* PARAMS */
        vector<string> ports;
        string host;
        string bodySize;
        ssize_t maxBodySize;
        vector<string> serverNames;
        map<int, string> errorPages;
        map<string, Location> locations;

        /* WHI HA */
        string getErrorPage(string code);
        string getLocations(string path);
        ServerConfiguration getServerConfiguration(const string &host, string &port, string &ip);
};

#endif /* ayeh ayeh, config.hpp */
