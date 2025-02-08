#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "Server.hpp"
#include "ConfigurationParser.hpp"
#include "ClientHandler.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include <exception>
#include <map>
#include <vector>

/*
 * Cluster class
 *
 * Manages server initialization, event processing through epoll, and connection handling.
 */
class Cluster
{
    public:
        /*
        * Cluster constructor
        *
        * Initializes the cluster by creating an epoll instance and initializing servers.
        *
        * @param config: Configurations object containing server settings.
        * @throws runtime_error if epoll creation or server initialization fails.
        */
        Cluster(Configurations &config);
        
        /*
        * Cluster destructor
        *
        * Ensures proper cleanup of cluster resources.
        */
        ~Cluster();

        /*
        * Run the cluster event loop.
        *
        * Continually listens for and processes events on registered sockets.
        */
        void run();

    private:
        // object to store configurations from ConfigurationParser
        Configurations &config;

        // epoll instance file descriptor used for event monitoring
        int epoll_fd;

        // Vector storing pointers to initialized Server objects
        std::vector<Server*> servers;

        // Map associating client file descriptors with their corresponding ClientHandler instances
        std::map<int, ClientHandler*> clientsZone;

        // Map mapping server file descriptors to their associated Server objects
        std::map<int, Server*> server_fd_to_server;

        // Map linking client file descriptors to the file descriptor of the server they are connected to
        std::map<int, int> client_to_server;

        /*
        * Create epoll instance
        *
        * Uses epoll_create to create an epoll instance used for event notification.
        *
        * @throws runtime_error if epoll creation fails.
        */
        void createEpoll();

        /*
        * Initialize servers
        *
        * Iterates through provided configurations to create and initialize Server objects.
        *
        * @throws runtime_error if no valid servers are available.
        */
        void initializeServers();

        /*
        * Add a socket to the epoll instance
        *
        * Registers a file descriptor with the epoll instance for monitoring.
        *
        * @param fd: File descriptor to add.
        * @throws runtime_error if adding socket fails.
        */
        void addSocketToEpoll(int fd);

        /*
        * Process epoll events
        *
        * Retrieves and handles events via the epoll instance.
        *
        * @param events: Array of epoll_event to process.
        * @throws runtime_error if epoll_wait fails.
        */
        void processEvents(struct epoll_event *events);

        /*
        * Accept new connections on a server socket
        *
        * Accepts incoming connections and registers the resulting client socket.
        *
        * @param serverSocket: The server socket file descriptor.
        * @throws runtime_error if accepting a connection fails.
        */
        void acceptConnections(int serverSocket);

        /*
        * Handle events on a client socket
        *
        * Based on the event data, delegates to reading or writing functions.
        *
        * @param fd: Client file descriptor.
        * @param eventsData: Bitmask representing the triggered events.
        */
        void handleClientEvent(int fd, uint32_t eventsData);

        /*
        * Handle read events for a client
        *
        * Processes incoming data on the client socket.
        *
        * @param client: Pointer to the client handler.
        */
        void handleReadEvent(ClientHandler* client);

        /*
        * Handle write events for a client
        *
        * Manages sending data out over the client socket.
        *
        * @param client: Pointer to the client handler.
        */
        void handleWriteEvent(ClientHandler* client);

        /*
        * Cleanup a client connection
        *
        * Removes a client from monitoring and frees associated resources.
        *
        * @param it: Iterator pointing to the client in the clientsZone map.
        */
        void cleanupClient(std::map<int, ClientHandler*>::iterator it);

        /*
        * Check if a file descriptor belongs to a server socket
        *
        * @param fd: File descriptor to check.
        * @return true if the fd corresponds to a server socket; false otherwise.
        */
        bool isServerFd(int fd) const;

        /*
        * Retrieve the server associated with a client file descriptor
        *
        * @param fd: Client file descriptor.
        * @return Reference to the corresponding Server.
        * @throws runtime_error if the server is not found.
        */
        Server& getServerByClientFd(int fd);

        /*
        * Retrieve the server by its file descriptor
        *
        * @param fd: Server file descriptor.
        * @return Reference to the corresponding Server.
        * @throws runtime_error if the server is not found.
        */
        Server& getServerByFd(int fd);

        /*
        * Cleanup a socket and remove it from the epoll monitoring
        *
        * Closes the socket file descriptor and cleans up associated data.
        *
        * @param fd: File descriptor to cleanup.
        */
        void cleanupSocket(int fd);

        /*
        * Cleanup all cluster resources
        *
        * Closes all open sockets, deallocates servers, and cleans up the epoll instance.
        */
        void cleanup();
};

#endif // CLUSTER_HPP