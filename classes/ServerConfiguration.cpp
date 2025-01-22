#include "../includes/ServerConfiguration.hpp"
#include "ServerConfiguration.hpp"

ServerConfiguration::ServerConfiguration() : maxBodySize(-1) {}

ServerConfiguration::ServerConfiguration(ServerConfiguration *other)
    : ports(other->ports),
      host(other->host),
      bodySize(other->bodySize),
      maxBodySize(other->maxBodySize),
      serverNames(other->serverNames),
      errorPages(other->errorPages),
      locations(other->locations)
{
}

bool ServerConfiguration::hasLocation(std::string &location)
{
	if (locations.find(location) != locations.end())
		return true;
    
	for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		size_t i = 0;
		while (location[i] && it->first[i] && location[i] == it->first[i])
			++i;
		if (!it->first[i] && (location[i] == '/' || !location[i]))
			return true;
	}
	if (locations.find("/") != locations.end())
		return true;
	return false;
}

Location& ServerConfiguration::getLocation(std::string &location)
{
	Location *loc = NULL;
	int maxMatch = 0;
	for (std::map<std::string, Location>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		size_t i = 0;
		while (location[i] && it->first[i] && location[i] == it->first[i])
			++i;
		if (!it->first[i] && (location[i] == '/' || !location[i]) && i > maxMatch)
		{
			loc = &it->second;
			maxMatch = i;
		}
	}
	if (loc == NULL)
		return locations["/"];
	return *loc;
}

bool Location::thereIsMethod(std::string &method)
{
	if (allow_methods.empty() && method == "GET")
		return true;
    
	std::vector<std::string>::iterator it = std::find(allow_methods.begin(), allow_methods.end(), method);
	return it != allow_methods.end();
}

bool Location::thereIsCGI(std::string &cgi)
{
	return cgi_path.find(cgi) != cgi_path.end();
}