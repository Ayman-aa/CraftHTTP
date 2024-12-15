#include "includes/Server.hpp"

int main() {
    ServerConfiguration server;
    server.ports.push_back("8080");
    server.ports.push_back("8081");
    server.host = "127.0.0.1";
    server.locations["/"].root = "/home/aymanisme/CraftHTTP/assets"; // Correct root directory

    // Define custom error pages
    server.errorPages["404"] = "/home/aymanisme/CraftHTTP/errorPages/404.html";
    server.errorPages["405"] = "/home/aymanisme/CraftHTTP/errorPages/405.html";

     cout << "Server host: " << server.host <<  endl;
     cout << "Root of location /: " << server.locations["/"].root <<  endl;

    Server serverInstance(server);
    serverInstance.run();

    return 0;
}