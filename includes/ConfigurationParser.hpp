#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "ServerConfiguration.hpp"

class ConfigurationParser
{
    public:
        vector<ServerConfiguration> servers;
        ConfigurationParser(){
            servers = vector<ServerConfiguration>();
        };
};
typedef ConfigurationParser ClusterConfiguration;
#endif