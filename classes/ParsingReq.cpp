#include "../includes/ParsingReq.hpp"
ParsingReq::ParsingReq()
{
    this->contentLength = 0;
    this->iscgi = false;
    this->haeaderGood = false;
}
std::vector<std::string> splitwithEOH(std::string &str)
{
    //we split the string with the end of the header "\r\n" or "\n";
    std::vector<std::string> lines;
    size_t pos = 0;
    while (pos < str.length())
    {

        size_t two = str.find("\r\n", pos);
        size_t one = str.find("\n", pos);
        size_t end = std::min(one, two);
        if (end == std::string::npos)
            break;
        lines.push_back(str.substr(pos, end - pos));
        if(end == one)
            pos = end + 1;
        else
        pos = end + 2;
    }
    return lines;
}
std::vector<std::string> splits(std::string &str,std::string deli = " ")
{
    std::vector<std::string> words;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t end = str.find(deli, pos);
        if (end == std::string::npos)
            {
                words.push_back(str.substr(pos));
                break;
            }
        words.push_back(str.substr(pos, end - pos));
        pos = end + 1;
    }
    return words;
}
std::string expand(std::string&input)
{
    std::string res = "";
    for(size_t i =0; i < input.length();i++)
    {
        if(input[i]=='%' && i + 2 < input.size())
        {
            int hexValue;
            if(sscanf(input.substr(i + 1, 2).c_str(), "%x", &hexValue) == 1)
            {
                res += static_cast<char>(hexValue);
                i +=2;
            }
            else
            {
                res += input[i];
            }
        } else if(input[i] == '+')
        {
            res += ' ';
        } else
        {
            res += input[i];
        }
    }
    return res;
}
void ParsingReq::parsRequestline(std::string &line)
{
    //parse the request line
    //if something is wrong check that you need to trim !!!
    std::vector<std::string> words = splits(line);
    if(line[0] != ' ' && words.size() == 3)
    {
        msg.method = words[0];
        msg.uri.unparsedURI = words[1];
        std::string httpversion = words[2];
        msg.uri.fullUri = expand(msg.uri.unparsedURI);
        words = splits(httpversion, "/");
        if(words.size() != 2 || words[0] != "HTTP")
            throw HttpError(BadRequest, "Bad Request");
        if(words[1] != "1.1")
            throw HttpError(HTTPVersionNotSupported,"505 HTTP Version Not Supported");
    }
    else
        throw HttpError(BadRequest, "Bad Request");
}
void ParsingReq::parseHeaders(std::string &line)
{
    uint64_t pos = line.find(":");
    std::string key = line.substr(0,pos);
    std::string value = (pos != std::string::npos) ? line.substr(pos +1) : "";
    if(key.find(' ') != std::string::npos)
        throw HttpError(BadRequest, "Bad Request");
    if(key == "Host" && msg.headers.find("Host") != msg.headers.end())
        throw HttpError(BadRequest, "Bad Request");
    
}
int ParsingReq::loadHeaders(Content &data)
{
    //skip new lines
    while(data.toStr().length() && data.toStr()[0] == '\n')
        data.erase(0,1);
    //find the end of the header
    size_t EOH = std::min(data.toStr().find("\r\n\r\n"), data.toStr().find("\n\n"));
    if (EOH != std::string::npos) 
    {
        std::string pack = data.toStr().substr(0, EOH);
        std::vector<std::string> lines = splitwithEOH(pack);
        parsRequestline(lines[0]);
        for (size_t i = 1; i < lines.size(); i++)
        {
                parseHeaders(lines[i]);
        }
        this->haeaderGood = true;
        size_t start = EOH == data.find("\r\n\r\n") ? EOH + 4 : EOH + 2;
        data = data.substr(start, EOH - start);
    }
    return 0;
}
void ParsingReq::checkPath()
{

	struct stat fileInfo;
	if (access(fullLocation.c_str(), F_OK) == 0){
		stat(fullLocation.c_str(), &fileInfo);
		if (S_ISDIR(fileInfo.st_mode))
			isdir = 1;
		if (access(fullLocation.c_str(), R_OK) != 0)
			throw HttpError(Forbidden, "Forbidden");
	}
	else
		throw HttpError(NotFound, "Not Found");
}
std::string getFileExtention(const std::string& filePath) {
	size_t pos = filePath.find_last_of(".");
	if (pos == std::string::npos)  // No extension.
		return "";
	return filePath.substr(pos + 1);  // Return everything after the period.
}

bool ParsingReq::parseUri(const std::string &uriStr)
{
    uriE &uri = msg.uri;
    size_t queryPos = uriStr.find('?');
    uri.path = uriStr.substr(0, queryPos);
    if(queryPos != std::string::npos){
        query = uriStr.substr(queryPos + 1);
    }
    // loc = serverConfig.getLocation(msg.uri.path);
    if (this->loc.upload_path.empty())
        this->upload_path = "./www/UPLOADS";
    else
        this->upload_path = this->loc.upload_path;
    fullLocation = loc.root + msg.uri.path;
    if (!loc.redirection_return.empty())
        return 1;
    checkPath();
    //perpre for cgi extension
    std::string fileExtention = getFileExtention(msg.uri.path);
    if (loc.cgi_path.find(fileExtention) != loc.cgi_path.end()){
        cgi = loc.cgi_path[fileExtention];
        iscgi = true;
    }
    return 0;
}
void ParsingReq::parseRequest()
   {
  	std::map<std::string, std::string>::iterator  trans=msg.headers.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator  con=msg.headers.find("Content-Length");
    if(msg.method != "GET" && msg.method != "POST" && msg.method != "DELETE")
      	throw HttpError(MethodNotAllowed, "Method Not Allowed");
    if (parseUri(msg.uri.fullUri))
          return;//checking redirection;
    if (serverConfig.hasLocation(msg.uri.path))
    {
        if (!serverConfig.getLocation(msg.uri.path).thereIsMethod(msg.method))
            throw HttpError(MethodNotAllowed, "Method Not Allowed");
    }
    else
        throw HttpError(NotFound, "Not Found");
    if (msg.method == "POST")
    {
        if (trans != msg.headers.end())
        {  
            if (trans->second != "chunked")
                throw HttpError(NotImplemented, "Not Implemented");
        }
        else if (con == msg.headers.end())
            throw HttpError(LengthRequired, "Length Required");
        else if (!isValidBase(con->second, this->contentLength, 10))
            throw HttpError(BadRequest, "Bad Request");
        if(!allCharactersAllowed(msg.uri.unparsedURI, VALID_CHARS))
            throw HttpError(BadRequest, "Bad Request");
        if (msg.uri.unparsedURI.size() > 2048)
            throw HttpError(RequestURIToLong, "Request-URI Too Long");
    }
  }
// int main() {
//     std::string path ="f";
//  struct   stat  s;
//     if(access(path.c_str(), F_OK) == 0)
//     {
//         stat(path.c_str(), &s);
//         std::cout <<"File user id: " << s.st_uid << std::endl;
//         std::cout << "File size: " << s.st_size << std::endl;
//     }
 
//     return 0;
// }