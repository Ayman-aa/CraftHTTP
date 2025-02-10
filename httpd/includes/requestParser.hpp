/* -- RequestParser.hpp -- */

#ifndef REQUEST_PARSER_HPP
#define REQUEST_PARSER_HPP

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include "content.hpp"
#include "httpError.hpp"
#include "tools.hpp"

struct Uri {
    std::string unparsedURI;
    std::string path;
    std::string fullUri;
};

typedef struct Message {
    std::string method;
    Uri uri;

    std::map<std::string, std::string> headers;
} message;

class RequestParser {
private:
	bool headersLoaded;
    size_t contentLength;
	message request;    

// Private helper functions you'll need to implement
    void checkHeader(std::string& header);
    void checkRequestLine(std::string& requestLine);
	bool checkHttpVersion(std::string& token);

public:
    RequestParser();
    
    // Main parsing functions
    int loadHeaders(Content& data);
    void parseRequest();
    
    // Getters
    const std::string& getMethod() const;
    const Uri& getUri() const;
    const std::map<std::string, std::string>& getHeaders() const;
};

#endif
