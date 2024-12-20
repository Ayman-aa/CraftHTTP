#include "includes/Cluster.hpp"
#include "includes/ServerConfiguration.hpp"

int main() {
    ServerConfiguration config;
    config.ports.push_back("8080");
    config.ports.push_back("8081");
    config.host = "127.0.0.1";
    config.locations["/"].root = "/mnt/c/Users/Ayman/Desktop/crafthttp/assets"; // Correct root directory

    // Define custom error pages
    config.errorPages["404"] = "/mnt/c/Users/Ayman/Desktop/crafthttp/errorPages/404.html";
    config.errorPages["405"] = "/mnt/c/Users/Ayman/Desktop/crafthttp/errorPages/405.html";

    std::cout << "Server host: " << config.host << std::endl;
    std::cout << "Root of location /: " << config.locations["/"].root << std::endl;

    Cluster cluster(config);
    cluster.run();

    return 0;
}
