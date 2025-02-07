#pragma once
#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <unistd.h>
#include <functional>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctime>
#include <vector>
#include <map>
#include "RequestParser.hpp"
#include "tools.hpp"
#include "ConfigurationParser.hpp"
#include "ServerConfiguration.hpp"
#include "Content.hpp"

/**
 * Constant for the default buffer size used in socket operations.
 * This buffer size is used to allocate temporary storage when reading data from sockets.
 */
const int BUFFER_SIZE = 1024;

/**
 * Enum representing the current status of a client connection.
 */
enum ClientStatus
{
    // The connection is currently receiving data.
    Receiving, 
    // The connection is in the process of sending data.
    Sending,
    // The connection has been closed.
    Closed, 
};

/**
 * Enum representing the different multipart/form-data processing states.
 */
enum MultiPartState
{
    // The initial state, waiting for the start boundary.
    startBound,
    // Processing the Content-Disposition header in multipart data.
    ContentDisposition,
    // Reading the file content section of multipart data.
    FileContent,
    // Detected the end boundary of multipart data.
    EndBound,
};

/**
 * Struct representing a client connection.
 * Contains the socket file descriptor and the timestamp of the last activity,
 * which can be used for connection timeouts.
 */
struct Connection
{
    // Socket file descriptor for the client connection.
    int sockfd;
    // Timestamp of the last activity on the connection.
    std::time_t lastActivity;
};

/**
 * ClientHandler class
 *
 * Inherits from RequestParser to handle HTTP request parsing.
 * Manages the client connection, including reading from and writing to the socket,
 * processing various HTTP methods (GET, POST, DELETE), and handling multipart data and CGI.
 */
class ClientHandler : public RequestParser
{
	private:
		// Epoll instance file descriptor used for monitoring client socket events.
		int epollFd;
		
		// Name of the file being processed in a POST operation.
		std::string postedFileName; 
		
		// List of temporary file paths created during data processing.
		std::vector<std::string> tmpFiles;
		
	public:
		// Current status of the client connection.
		ClientStatus status;
		
		// Buffer storing incoming request data.
		Content readingBuffer;
		
		// Buffer storing outgoing response data.
		Content sendingBuffer;
		
		// Client's socket file descriptor.
		int clientFd;
		
		// Server-specific configuration for this client session.
		ServerConfiguration ServerConfig;
		
		// Reference to global configurations.
		Configurations &Configs;
		
		// Timestamp of the last received data from the client.
		std::time_t lastReceive;
		
		// Timestamp when the first data was received from the client.
		std::time_t firstReceive;
		
	private:
		/**
		 * Sends data from the sending buffer to the client's socket.
		 */
		void sendToSocket();
		
		/**
		 * Reads data from the client's socket into the reading buffer.
		 *
		 * @param bufferSize Size of the temporary buffer used for each read operation.
		 */
		void readFromSocket(int bufferSize = BUFFER_SIZE);

		// Auxiliary counter for internal operations.
		unsigned int counter;
		
		// Size of the current chunk when processing chunked transfer encoding.
		unsigned int chunkSize;
		
		// Flag indicating if a certain operation is in progress.
		bool in;
		
		// Boundary string used in multipart/form-data processing.
		std::string boundary;
		
		// Current processing state for multipart requests.
		MultiPartState state;
		
	public:
		/**
		 * Constructs a new ClientHandler.
		 *
		 * Initializes the client handler with the given socket file descriptor, epoll descriptor,
		 * server-specific configuration, and global configurations. Sets up initial state values.
		 *
		 * @param clientFd The client's socket file descriptor.
		 * @param epollFd The epoll instance file descriptor.
		 * @param ServerConfig The server configuration for this connection.
		 * @param Configs Global configurations.
		 */
		ClientHandler(int clientFd, int epollFd, const ServerConfiguration &ServerConfig, Configurations &Configs);
		
		/**
		 * Destroys the ClientHandler.
		 *
		 * Cleans up any temporary files created during the client's session.
		 */
		~ClientHandler();
		
		/**
		 * Prepares the handler to receive incoming data.
		 *
		 * Reads data from the client socket, processes headers,
		 * and triggers appropriate request parsing actions.
		 */
		void readyToReceive();
		
		/**
		 * Prepares the handler to send a response.
		 *
		 * Sends data from the sending buffer to the client, and monitors for CGI output if needed.
		 */
		void readyToSend();
		
	public:
		// Path of the file to serve or process.
		std::string file;
		
		// HTTP status code to be sent in the response.
		std::string statusCode;
		
		// HTTP status string to be sent in the response.
		std::string statusString;
		
		// Additional headers to include in the response.
		std::string extraHeaders;
		
		// Flag indicating whether the response is generated by a CGI script.
		bool isCGI;		
		
		// Flag indicating whether response headers have been sent.
		bool headersSent;
		
		/**
		 * Sets the parameters for the HTTP response.
		 *
		 * @param statusCode The HTTP status code.
		 * @param statusString The HTTP status description.
		 * @param extraHeaders Additional headers to include.
		 * @param file The file to serve.
		 * @param isCGI Optional flag indicating if CGI is used (default is false).
		 */
		void setResponseParams(std::string statusCode, std::string statusString, std::string extraHeaders, std::string file, bool isCGI=false);
		
		/**
		 * Sends a server error response to the client.
		 */
		void sendServerError();
		
		/**
		 * Sends the HTTP response to the client.
		 */
		void SendResponse();
		
		/**
		 * Generates the HTTP response headers.
		 *
		 * @return A string containing the generated headers.
		 */
		std::string generateHeaders();
		
		/**
		 * Retrieves the file extension of the requested resource.
		 *
		 * @return A string representing the file extension.
		 */
		std::string getExtension();
		
		/**
		 * Retrieves the Content-Length as a string.
		 *
		 * @return A string representing the Content-Length.
		 */
		std::string getContentLength();
		
		/**
		 * Retrieves the MIME type based on the given file extension.
		 *
		 * @param ext The file extension.
		 * @return A string representing the MIME type.
		 */
		std::string getMimeType(std::string ext);
		
		/**
		 * Retrieves the file extension for a given MIME type (used in POST responses).
		 *
		 * @param mimeType The MIME type.
		 * @return A string representing the file extension.
		 */
		std::string getExtensionPost(std::string mimeType);
		
		// Offset used during data handling.
		int offset; 
		
		/**
		 * Handles HTTP POST requests.
		 */
		void PostMethod();
		
		/** 
		 * Processes data using chunked transfer encoding.
		 */
		void ChunkedHandler();
		
		/**
		 * Handles regular (non-chunked) data transfers.
		 */
		void RegularDataHandler();
		
		/**
		 * Processes multipart/form-data, handling file uploads and multiple boundaries.
		 */
		void MutiplePartHandler();
		
		/**
		 * Handles HTTP GET requests.
		 */
		void GetMethod();
		
		/**
		 * Determines the index file for directory requests.
		 *
		 * @return An integer index indicating the file order.
		 */
		int GetIndex();
		
		/**
		 * Generates an auto-index page for directory listings.
		 */
		void GetAutoIndex();
		
		/**
		 * Redirects the client to another URL as specified in the server configuration.
		 */
		void redirect();
		
		/**
		 * Handles HTTP DELETE requests.
		 */
		void DeleteMethod();
		
		// CGI output generated after script execution.
		std::string CGIoutput;
		
		// Flag indicating whether a CGI process is being monitored.
		bool monitorCGI;
		
		// Process ID of the running CGI script.
		pid_t CGIpid;
		
		// Timestamp when the CGI process was started.
		std::time_t CGIstartTime;
		
		/**
		 * Executes the configured CGI script.
		 */
		void execCGI();
		
		/**
		 * Monitors the CGI process for output or timeout.
		 */
		void checkCGI();

};

#endif //EVENT_HANDLER_HPP