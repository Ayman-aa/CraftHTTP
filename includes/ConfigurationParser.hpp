#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "ServerConfiguration.hpp"

class ConfigurationParser
{
    public:
        vector<ServerConfiguration> servers;
};
typedef ConfigurationParser ClusterConfiguration;
#endif