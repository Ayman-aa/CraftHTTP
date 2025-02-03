#include "includes/Cluster.hpp"
#include "includes/ServerConfiguration.hpp"
#include "includes/ConfigurationParser.hpp"

int main() {
    try{

    
    string configFile = "test.yaml";
    ConfigurationParser config(configFile);
    Cluster cluster(config);
    cluster.run();
    }
    catch (const exception& e) {
        cerr << e.what() << '\n';
    }

    return 0;
}
