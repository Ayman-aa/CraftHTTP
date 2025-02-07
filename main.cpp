#include "includes/Cluster.hpp"
#include "includes/ServerConfiguration.hpp"
#include "includes/ConfigurationParser.hpp"

int main(int argc, char const *argv[]) 
{
   std::string configFile = "default.yaml";
    if (argc != 2) 
        std::cerr << "Usage: ./webserv <config_file>, will launch with default setting" << std::endl;
    else
        configFile = argv[1];
    try
    {
        ConfigurationParser config(configFile);
        Cluster cluster(config);
        cluster.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
