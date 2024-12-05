#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "ServerConfiguration.hpp"

class ConfigurationParser
{
    private:
        void configurationInitiation(string configFile);
        void parse_file(ifstream &file);
        
        ConfigurationParser();
    public:
        ConfigurationParser(string configFile);
        ~ConfigurationParser();
        ServerConfiguration getConfiguration(const char *filename);
};

#endif