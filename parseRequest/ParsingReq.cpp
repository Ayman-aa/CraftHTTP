#include "ParsingReq.hpp"
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
int ParsingReq::loadHeaders(Binary &data)
{
    //skip new lines
    while(data.to_string().length() && data.to_string()[0] == '\n')
        data.erase(0,1);
    //find the end of the header
    size_t EOH = std::min(data.to_string().find("\r\n\r\n"), data.to_string().find("\n\n"));
    if (EOH != std::string::npos) 
    {
        std::string pack = data.to_string().substr(0, EOH);
        std::vector<std::string> lines = splitwithEOH(pack);
        parsRequestline(lines[0]);
        for (size_t i = 1; i < lines.size(); i++)
        {
                parseHeaders(lines[i]);
        }
        this->headersLoaded = true;
        size_t start = headerEnd == data.find("\r\n\r\n") ? headerEnd + 4 : headerEnd + 2;
        data = data.substr(start, headerEnd - start);
    }
    return 0;
}
void ParsingReq::parseRequest()
   {
  	std::map<std::string, std::string>::iterator it trans=msg.headers.find("Transfer-Encoding");
	std::map<std::string, std::string>::iterator it con=msg.headers.find("Content-Length");
    if(msg.method != "GET" && msg.method != "POST" && msg.method != "DELETE")
      	throw HttpError(MethodNotAllowed, "Method Not Allowed");
  }
// int main() {
//     std::string testStr = "Header1: value1\r\nHeader2: value2\r\nHeader3: value3\r\n";
    
//     std::vector<std::string> myresut1 = splitwithEOH(testStr);
//     std::vector<std::string> their= splitWithDelimiters(testStr, {"\n", "\r\n"});
//     for(size_t i = 0; i < their.size(); i++)
//         std::cout << their[i] << " " << their.size() << std::endl;
//     for(size_t i = 0; i < myresut1.size(); i++)
//         std::cout << myresut1[i] << " " << myresut1.size() << std::endl;
    

//     return 0;
// }