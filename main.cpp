#include "includes/Cluster.hpp"
#include "includes/ServerConfiguration.hpp"
#include "includes/ConfigurationParser.hpp"

int main() {
    try{

    
    string path = "test.yaml";
    ConfigurationParser config(path);
    // config.servers.resize(2);
    // config.servers[0].ports.push_back(8080);
    // config.servers[0].ports.push_back(8081);
    // config.servers[0].host = "127.0.0.1";
    // config.servers[0].locations["/"].root = "/mnt/c/Users/Ayman/Desktop/crafthttp/assets"; // Correct root directory
    // config.servers[0].errorPages[404] = "/mnt/c/Users/Ayman/Desktop/crafthttp/errorPages/404.html";
    // config.servers[0].errorPages[405] = "/mnt/c/Users/Ayman/Desktop/crafthttp/errorPages/405.html";
    
    // config.servers[1].ports.push_back(8082);
    // config.servers[1].ports.push_back(8083);
    // config.servers[1].host = "127.0.0.2";
    // config.servers[1].locations["/"].root = "/mnt/c/Users/Ayman/Desktop/crafthttp/assets/root"; // Correct root directory
    // config.servers[1].errorPages[404] = "/mnt/c/Users/Ayman/Desktop/crafthttp/errorPages/404.html";
    // config.servers[1].errorPages[405] = "/mnt/c/Users/Ayman/Desktop/crafthttp/errorPages/405.html";

    std::cout << "Server host 1: " << config.servers[0]->host << std::endl;
    std::cout << "Root of location 1 /: " << config.servers[0]->locations["/"].root << std::endl;

    cout << "Server host 2: " << config.servers[1]->host << endl;
    cout << "Root of location 2 /: " << config.servers[1]->locations["/"].root << endl;

    Cluster cluster(config);
    cluster.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
