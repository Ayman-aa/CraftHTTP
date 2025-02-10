/* -- requestParser.cpp -- */

#include "../includes/requestParser.hpp"

RequestParser::RequestParser(): headersLoaded(false), contentLength(0) {}

int RequestParser::loadHeaders(Content& data) 
{
	std::string requestStr = data.toStr();
	std::vector<std::string> lines;
	
	std::istringstream stream(requestStr);
	std::string line;
	
	int j = 0;

	while (getline(stream, line, '\n')) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		lines.push_back(line);	
		std::cout<<"[" <<j<< "]: " << "[" << lines[j]<<"]" << std::endl;
		j++;
	}

	for (size_t i = 0; i < lines.size(); i++)
	{
		if (i == 0) checkRequestLine(lines[i]);
		//else checkHeader(lines[i]);
	}

	return 1;
}

void RequestParser::checkRequestLine(std::string& requestLine)
{
	std::cout << "Allo ma man" << std::endl;
	std::istringstream stream(requestLine);
	std::string token;
	
	int i = 0;

	while (getline(stream, token, ' ')) {
		std::cout << "i = " << i << std::endl;
		if (i == 0) request.method = token; /* Hahua rak storiti */
		if (i == 1) request.uri.unparsedURI = token;
		if (i == 2) {
			if (!checkHttpVersion(token))
				throw HttpError(HTTPVersionNotSupported, "505 HTTP Version Not Supported");
		}
		i++;
		std::cout << "token: [" << token << "]" << std::endl;
	}
}

bool  RequestParser::checkHttpVersion(std::string& token)
{
	std::cout << "i' m in checkHttpVersion() function" << std::endl;
	std::vector<std::string> httpVersion = strSplit(token, "/", 0);
	
	std::cout << "httpVersion[0]: [" << httpVersion[0] << "]" << std::endl;
	std::cout << "httpVersion[1]: [" << httpVersion[1] << "]" << std::endl;
	
	if (httpVersion[0] != "HTTP") return false;
	if (httpVersion[1] != "1.0") return false;
	return true;
}
