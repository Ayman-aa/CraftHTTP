#ifndef SERVER_HPP
#define SERVER_HPP

#include "ServerConfiguration.hpp"
#include <map>
#include <vector>
#include <set>
#include <string>
#include <netdb.h>

/*
 * Server class
 *
 * Responsible for creating and managing server sockets based on the provided configuration.
 * It initializes sockets for each specified port, configures them with necessary options,
 * binds them to the appropriate network interfaces, and prepares them for listening.
 * On failure during any of these operations, the class ensures cleanup and throws a runtime_error.
 */
class Server
{
    public:
        /**
         * Constructor initializing the Server with its configuration.
         *
         * Attempts to create server sockets for each port specified in the configuration.
         * On failure, cleans up and throws a runtime_error.
         *
         * @param config Pointer to the ServerConfiguration object.
         */
        Server(ServerConfiguration* config);

        /**
         * Virtual destructor ensuring proper cleanup of resources.
         */
        virtual ~Server();

        /**
         * Returns the main server socket file descriptor used for accepting connections.
         *
         * @return File descriptor of the main server socket.
         */
        int getSocket() const;

        /**
         * Returns a map associating file descriptors with corresponding port strings.
         *
         * @return const reference to the map of file descriptors to port strings.
         */
        const std::map<int, std::string>& getFdToPort() const;

        /**
         * Provides access to the server's configuration settings.
         *
         * @return Reference to the ServerConfiguration object.
         */
        ServerConfiguration& getConfig();

        /**
         * Returns a vector containing all server socket file descriptors.
         *
         * @return const reference to the vector of socket file descriptors.
         */
        const std::vector<int>& getSockets() const;

        // Set containing file descriptors of all currently connected clients.
        std::set<int> connectedClients;

    private:
        // Main server socket file descriptor used for accepting connections.
        int main_socket; 

        // Server configuration containing listening ports, host, and other settings.
        ServerConfiguration config;

        // Vector of server socket file descriptors created from configuration.
        std::vector<int> sockets;

        // Map linking socket file descriptors to their corresponding port strings.
        std::map<int, std::string> fd_to_port;

        /**
         * Retrieves address information for the server based on a given port.
         *
         * Uses getaddrinfo with the AI_PASSIVE flag to obtain a list of potential addresses.
         *
         * @param port The port for which to retrieve address information.
         * @return Pointer to the addrinfo struct containing address details.
         * @throws runtime_error if getaddrinfo fails.
         */
        struct addrinfo* serverInfo(const std::string& port);

        /**
         * Binds the provided socket file descriptor to the address info and port.
         *
         * @param sockFd Socket file descriptor.
         * @param info Pointer to addrinfo structure with address details.
         * @param port Port as astd::string.
         * @throws runtime_error if bind fails.
         */
        void bindSocket(int sockFd, struct addrinfo *info, const std::string &port);

        /**
         * Configures the socket to listen, with a specified backlog.
         *
         * @param sockFd Socket file descriptor.
         * @param port Port as astd::string.
         * @param backlog Maximum length of the queue of pending connections.
         * @throws runtime_error if listen fails.
         */
        void listenSocket(int sockFd, const std::string& port, int backlog);

        /**
         * Sets necessary socket options (e.g., non-blocking mode, address reusability, keep-alive).
         *
         * @param sockFd Socket file descriptor.
         * @throws runtime_error if setsockopt fails.
         */
        void setSocketOptions(int sockFd);

        /**
         * Creates server sockets based on the provided configuration.
         *
         * For each specified port, attempts to create, set options, bind, and listen on a socket.
         * Stores the successfully created sockets in the sockets vector and maps them to ports.
         *
         * @throws runtime_error if binding to a port fails or no sockets are successfully created.
         */
        void createSockets();

        /**
         * Cleans up and deallocates resources associated with the server.
         *
         * Closes all open sockets and clears internal containers.
         */
        void cleanup();
};

#endif // SERVER_HPP