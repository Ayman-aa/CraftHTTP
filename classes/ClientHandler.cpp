#include "../includes/ClientHandler.hpp"


ClientHandler::ClientHandler(int clientFd, int epollFd, const ServerConfiguration &ServerConfig, Configurations &Configs)
    : epollFd(epollFd), clientFd(clientFd), ServerConfig(ServerConfig), Configs(Configs) {
    this->status = Receiving;

    this->counter = 0;
    this->isDir = 0;
    this->isCGIfile = 0;
    this->headersSent = 0;
    this->offset = 0;
    this->lastReceive = 0;
    this->firstReceive = std::time(0);
    this->chunkSize = 0;
    this->in = false;
    this->state = startBound;
    this->isCGI = 0;
    this->monitorCGI = 0;
}

ClientHandler::~ClientHandler(){
    std::vector<std::string>::iterator it = tmpFiles.begin();
    for (;it != tmpFiles.end(); it++){
        std::cerr << "Removing file: " << *it << std::endl;
        std::remove(it->c_str());
    }
}


void ClientHandler::readyToReceive() {
    try {
        if (!headersLoaded) {
            readFromSocket();
            loadHeaders(readingBuffer);
        }
        if (headersLoaded) {
            if (message.headers.find("Host") != message.headers.end()) {
                ServerConfig = Configs.getServerConfig(ServerConfig.host, ServerConfig.ports, message.headers["Host"].substr(0, message.headers["Host"].find(":")));
                RequestParser::ServerConfig = ServerConfig;
            } else {
                throw HttpError(BadRequest, "Bad Request");
            }
            parseRequest();
            if (!location.redirection_return.empty()) {
                redirect();
            } else if (message.method == "GET") {
                GetMethod();
            } else if (message.method == "DELETE") {
                DeleteMethod();
            } else if (message.method == "POST") {
                PostMethod();
            }
        }
    } catch (const HttpError& e) {
        headersSent = 0;
        std::cerr << "HTTP Error (" << e.getErrorCode() << "): " << e.what() << std::endl;
        setResponseParams(toString(e.getErrorCode()), e.what(), "", "", false);
    }
}


void ClientHandler::readyToSend() {
    try {
        if (!headersLoaded) {
            if (std::time(0) - firstReceive > 5) {
                headersLoaded = 1;
                throw HttpError(RequestTimeOut, "Request Time Out 1");
            }
        }
        if (status == Sending) {
            if (monitorCGI) {
                checkCGI();
            } else {
                SendResponse();
            }
        } else if (status == Receiving && lastReceive) {
            if (std::time(0) - lastReceive > 5) {
                std::cout << "time out" << std::endl;
                status = Sending;
                throw HttpError(RequestTimeOut, "Request Time Out");
            }
        }
    } catch (const HttpError& e) {
        headersSent = 0;
        int errorCode = static_cast<int>(e.getErrorCode());
        std::cerr << "HTTP Error (" << errorCode << "): " << e.what() << std::endl;
        setResponseParams(toString(e.getErrorCode()), e.what(), "", "", false);
    }
}

void ClientHandler::readFromSocket(int bufferSize) {
	unsigned char buffer[bufferSize];
	std::memset(buffer, 0, bufferSize);
	ssize_t bytesRead = recv(this->clientFd, buffer, bufferSize - 1, 0);

	if (bytesRead <= 0) {
		status = Closed;
		std::cout << "Connection closed by client\n";
		return ;
	}
	this->readingBuffer.append(buffer, bytesRead);
}


void ClientHandler::sendToSocket()
{
	size_t totalBytesSent = 0;
	while (totalBytesSent < sendingBuffer.toStr().size())
	{
		ssize_t sendBytes = send(this->clientFd, sendingBuffer.toStr().c_str() + totalBytesSent, sendingBuffer.toStr().size() - totalBytesSent, MSG_NOSIGNAL);
		if (sendBytes <= 0)
		{
			status = Closed;
			std::cerr << "Error Sending data: " << strerror(errno) << "\n";
			return ;
		}
		totalBytesSent += sendBytes;
	}
	this->sendingBuffer.erase(0, totalBytesSent);
}