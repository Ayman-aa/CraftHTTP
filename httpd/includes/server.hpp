/* -- server.hpp --*/

#ifndef SERVER_HPP
# define SERVER_HPP

#include "configParser.hpp"

class Server {
	private:
		ServerConfiguration *config;
		int main_socket;
		vector <int> sockets;
		map<int, string> fd_to_port;

		struct addrinfo* serverInfo(const string& port);
		void bindSocket(int sockFD, struct addrinfo *info, const std::string &port);
		void listenSocket(int sockFD, const string& port, int backlog);
		void setSocketOptions(int sockFD);
		void createSockets();
		void cleanup();

	public:
		Server(ServerConfiguration* config);
		virtual ~Server();
		int getSocket() const;
		const map<int, string>& getFdToPort() const;
		const ServerConfiguration& getConfig() const;
		const vector<int>& getSockets() const;
};

#endif
