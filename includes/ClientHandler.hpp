#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include "ConfigurationParser.hpp"
#include "Content.hpp"
class ClientHandler 
{
    int epollFd;
    int clientFd;
    ServerConfiguration& serverConfig;
    Configurations& configs;
    public:
        Content readContent;
        ClientHandler(int clientFd, int epollFd, ServerConfiguration& serverConfig, Configurations& configs);
        void recieveRequest();
};

#endif