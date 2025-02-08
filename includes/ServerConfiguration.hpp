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
#include <algorithm>
#include "includes.hpp"

using namespace std;

/**
 * Struct representing a location block in the server configuration.
 */
struct Location
{
	// Whether autoindex is enabled for this location.
	bool autoindex;

	// Root directory for serving files.
	std::string root;

	// Path specifying the location.
	std::string path;

	// Redirection return URL or status code if specified.
	std::string redirection_return;

	// Upload path for storing uploaded files.
	std::string upload_path;

	// Allowed HTTP methods for this location.
	std::vector<std::string> allow_methods;

	// Default index files for directory requests.
	std::vector<std::string> index;

	// Mapping of CGI file extensions to their interpreter paths.
	std::map<std::string, std::string> cgi_path;

	/**
	 * Checks if a given method is allowed for this location.
	 *
	 * @param method HTTP method to check.
	 * @return true if the method is allowed; false otherwise.
	 */
	bool thereIsMethod(const std::string &method);

	/**
	 * Checks if a given CGI is configured for this location.
	 *
	 * @param cgi CGI identifier (e.g., file extension) to check.
	 * @return true if the CGI configuration exists; false otherwise.
	 */
	bool thereIsCGI(const std::string &cgi);
};

/**
 * Class representing server configuration settings.
 *
 * This class stores details such as ports, host, body size limits, server names,
 * custom error pages, and location-specific configurations.
 */
class ServerConfiguration
{
	public:
		/**
		 * Default constructor.
		 */
		ServerConfiguration();

		/**
		 * Copy constructor initializing configuration based on another instance.
		 *
		 * @param other Pointer to another ServerConfiguration object.
		 */
		ServerConfiguration(ServerConfiguration* other);

		// Vector of ports on which the server will listen.
		std::vector<std::string> ports;

		// Host address for the server.
		std::string host;

		// Body size as a string, as specified in the configuration.
		std::string bodySize;

		// Maximum allowed body size in bytes.
		ssize_t maxBodySize;

		// List of server names.
		std::vector<std::string> serverNames;

		// Mapping of HTTP error codes to custom error page paths.
		std::map<int, std::string> errorPages;

		// Mapping of location paths to their corresponding Location settings.
		std::map<std::string, Location> locations;

		/**
		 * Checks if a particular location exists in the configuration.
		 *
		 * @param location Location path to check.
		 * @return true if the location exists; false otherwise.
		 */
		bool hasLocation(std::string &location);

		/**
		 * Retrieves the Location configuration for the specified path.
		 *
		 * @param location Location path.
		 * @return Reference to the corresponding Location object.
		 */
		Location &getLocation(std::string &location);

		/**
		 * Retrieves the custom error page path for a given error code.
		 *
		 * @param errn_ Error code for which to obtain the error page.
		 * @return Error page path as a string.
		 */
		std::string getErrorPage(int errn_);
};

#endif