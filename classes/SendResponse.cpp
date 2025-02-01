#include "../includes/ClientHandler.hpp"

// response
void ClientHandler::SendResponse(){
	if (!headersSent){
		if (file.empty()){
			std::istringstream ss;
			int status;
			ss.str(statusCode);
			ss >> status;
			file = ServerConfig.getErrorPage(status);
		}
		if (access(file.c_str(), R_OK)){
			if (statusCode == "500"){
				sendServerError();
				return ;
			}
			throw HttpError(InternalServerError, "Internal Server Error");
		}
		std::string re;

		re = generateHeaders();

		sendingBuffer.append((const unsigned char *)re.c_str(),std::strlen(re.c_str()));
		sendToSocket();

		headersSent = 1;
	}
	else{
		std::ifstream fileToSend(file.c_str(), std::ios::binary);
		if (!fileToSend.is_open()){
			status = Closed;
			return ;
		}
			
		char buffer[BUFFER_SIZE + 1] =  {0};

		fileToSend.seekg(offset);
		fileToSend.read(buffer, BUFFER_SIZE);

		offset = fileToSend.tellg();
		int readLen = fileToSend.gcount();

		sendingBuffer.append((unsigned const char*)buffer, readLen);
		sendToSocket();

		if (fileToSend.eof())
			status = Closed;
		fileToSend.close();
	}
}

std::string ClientHandler::generateHeaders(){
	std::string re;

	re = "HTTP/1.1 " + statusCode + " " + statusString + "\r\n";
	re += "Content-Type: " + getMimeType(getExtension()) + "\r\n";
	if (!isCGI)
		re += "Content-Length: " + getContentLength() + "\r\n";
	re += extraHeaders;
	if (!isCGI)
		re += "\r\n";
	return re;
}

std::string ClientHandler::getExtension(){
	if (file.find(".") == std::string::npos)
		return "";
	std::vector<std::string> splitted = strSplit(file, ".", 1);
	return splitted[splitted.size() - 1];
}

std::string ClientHandler::getContentLength(){
	struct stat fileInfo;

	stat(file.c_str(), &fileInfo);

	std::stringstream ss;
	ss << fileInfo.st_size;

	return ss.str();
}

std::string ClientHandler::getMimeType(std::string ext){
	std::map<std::string, std::string> mimeTypes;

	mimeTypes["txt"] = "text/plain";
	mimeTypes["html"] = "text/html";
	mimeTypes["css"] = "text/css";
	mimeTypes["js"] = "application/javascript";
	mimeTypes["jpg"] = "image/jpeg";
	mimeTypes["webp"] = "image/webp";
	mimeTypes["png"] = "image/png";
	mimeTypes["gif"] = "image/gif";
	mimeTypes["pdf"] = "application/pdf";
	mimeTypes["doc"] = "application/msword";
	mimeTypes["xml"] = "application/xml";
	mimeTypes["json"] = "application/json";
	mimeTypes["mp3"] = "audio/mpeg";
	mimeTypes["csv"] = "text/csv";
	mimeTypes["zip"] = "application/zip";
	mimeTypes["tar"] = "application/x-tar";
	mimeTypes["mp4"] = "video/mp4";
	mimeTypes["ogg"] = "audio/ogg";
	mimeTypes["svg"] = "image/svg+xml";
	mimeTypes["xls"] = "application/vnd.ms-excel";
	mimeTypes["ppt"] = "application/vnd.ms-powerpoint";
	mimeTypes["ico"] = "image/x-icon";
	mimeTypes["woff"] = "font/woff";
	mimeTypes["mpg"] = "video/mpeg";

	std::map<std::string,std::string>::iterator it = mimeTypes.find(ext);
	if (it != mimeTypes.end())
		return it->second;
	return "text/plain";
}

std::string ClientHandler::getExtensionPost(std::string mimeType){
	std::map<std::string, std::string> mimeTypes;

	mimeTypes["text/plain"] = ".txt";
	mimeTypes["text/html"] = ".html";
	mimeTypes["text/css"] = ".css";
	mimeTypes["application/javascript"] = ".js";
	mimeTypes["image/jpeg"] = ".jpg";
	mimeTypes["image/webp"] = ".webp";
	mimeTypes["image/png"] = ".png";
	mimeTypes["image/gif"] = ".gif";
	mimeTypes["application/pdf"] = ".pdf";
	mimeTypes["application/msword"] = ".doc";
	mimeTypes["application/xml"] = ".xml";
	mimeTypes["application/json"] = ".json";
	mimeTypes["audio/mpeg"] = ".mp3";
	mimeTypes["text/csv"] = ".csv";
	mimeTypes["application/zip"] = ".zip";
	mimeTypes["application/x-tar"] = ".tar";
	mimeTypes["video/mp4"] = ".mp4";
	mimeTypes["audio/ogg"] = ".ogg";
	mimeTypes["image/svg+xml"] = ".svg";
	mimeTypes["application/vnd.ms-excel"] = ".xls";
	mimeTypes["application/vnd.ms-powerpoint"] = ".ppt";
	mimeTypes["image/x-icon"] = ".ico";
	mimeTypes["font/woff"] = ".woff";
	mimeTypes["video/mpeg"] = ".mpg";



	std::map<std::string,std::string>::iterator it = mimeTypes.find(mimeType);
	if (it != mimeTypes.end())
		return it->second;
	return ".tmp";
}

void ClientHandler::setResponseParams(std::string statusCode, std::string statusString, std::string extraHeaders, std::string file, bool isCGI){
	this->file = file;
	this->statusCode = statusCode;
	this->extraHeaders += extraHeaders;
	this->statusString = statusString;
	this->isCGI = isCGI;
	status = Sending;
}


void ClientHandler::sendServerError(){ 
    // Content to be sent in the HTTP response body
const char* response =
		"HTTP/1.1 500 Internal Server Error\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 1158\r\n\r\n"
		"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"\t<meta charset=\"UTF-8\">\n"
		"\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"\t<title>500 Internal Server Error</title>\n"
		"\t<style>\n"
		"\t\t:root {\n"
		"\t\t  --color-primary: #eee;\n"
		"\t\t  --color-inverted: #222;\n"
		"\t\t}\n"
		"\n"
		"\t\t@media (prefers-color-scheme: dark) {\n"
		"\t\t  :root {\n"
		"\t\t    --color-primary: #222;\n"
		"\t\t    --color-inverted: #aaa;\n"
		"\t\t  }\n"
		"\t\t}\n"
		"\n"
		"\t\thtml, body {\n"
		"\t\t  margin: 0;\n"
		"\t\t  padding: 0;\n"
		"\t\t  min-height: 100%;\n"
		"\t\t  height: 100%;\n"
		"\t\t  width: 100%;\n"
		"\t\t  background-color: var(--color-primary);\n"
		"\t\t  color: var(--color-inverted);\n"
		"\t\t  font-family: monospace;\n"
		"\t\t  font-size: 16px;\n"
		"\t\t  word-break: keep-all;\n"
		"\t\t}\n"
		"\n"
		"\t\t@media screen and (min-width: 2000px) {\n"
		"\t\t  html, body {\n"
		"\t\t    font-size: 20px;\n"
		"\t\t  }\n"
		"\t\t}\n"
		"\n"
		"\t\tbody {\n"
		"\t\t  display: flex;\n"
		"\t\t  justify-content: center;\n"
		"\t\t  align-items: center;\n"
		"\t\t}\n"
		"\n"
		"\t\tmain {\n"
		"\t\t  display: flex;\n"
		"\t\t}\n"
		"\n"
		"\t\tarticle {\n"
		"\t\t  display: flex;\n"
		"\t\t  align-items: center;\n"
		"\t\t  justify-content: center;\n"
		"\t\t  flex-direction: column;\n"
		"\t\t}\n"
		"\n"
		"\t\tarticle #error_text h1 {\n"
		"\t\t  font-size: 2em;\n"
		"\t\t  font-weight: normal;\n"
		"\t\t  padding: 0;\n"
		"\t\t  margin: 0;\n"
		"\t\t}\n"
		"\t  </style>\n"
		"</head>\n"
		"<body>\n"
		"\t<main>\n"
		"\t  <article>\n"
		"\t    <div id=\"error_text\">\n"
		"\t      <h1 class=\"source\">500 - Internal Server Error</h1>\n"
		"\t      <h1 class=\"target\"></h1>\n"
		"\t    </div>\n"
		"\t  </article>\n"
		"\t</main>\n"
		"</body>\n"
		"</html>";

	sendingBuffer.append((const unsigned char *)response,std::strlen(response));
	sendToSocket();
	status = Closed;
}