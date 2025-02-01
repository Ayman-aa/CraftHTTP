#include "../includes/ClientHandler.hpp"

void writeToFile(const std::string &data, const std::string& fileName)
{
    // std::cout << fileName << std::endl;
    std::cout << fileName.c_str() << std::endl;
    if (!freopen(fileName.c_str(), "ab", stdout))
        throw HttpError(InternalServerError, "Internal Server Error");
    std::cout << data;
    if (!freopen("/dev/tty", "ab", stdout))
        throw HttpError(InternalServerError, "Internal Server Error");
}

bool checkMultipart(std::string& str, std::string& boundary)
{
    size_t sep_pos = str.find(";");
    if (sep_pos == std::string::npos)
        return false;
    if (strtrim(str.substr(0, sep_pos)) != "multipart/form-data")
        return false;
    std::string bound = strtrim(str.substr(sep_pos + 1, str.size()));
    sep_pos = bound.find("=");
    if (sep_pos == std::string::npos)
        return false;
    if (strtrim(strtrim(bound.substr(0, sep_pos))) != "boundary")
        return false;
    boundary = strtrim(bound.substr(sep_pos + 1, bound.size()));
    if (boundary == "")
        return false;
    boundary = "--" + boundary;
    return true;
}

bool isContentDispositionValid(const std::string &str, std::string &fileName)
{
    size_t pos_1 = str.find("filename=");
    size_t pos_2;
    if (pos_1 != std::string::npos)
    {
        pos_2 = str.substr(pos_1 + 10, str.size()).find("\"");
        if (pos_2 == std::string::npos)
            return false;
        fileName = str.substr(pos_1 + 10, pos_2);
        if (fileName == "")
            return false;
        return true;
    }
    return false;
}

void ClientHandler::MutiplePartHandler()
{
    size_t pos;

    if (state == startBound)
    {

        if (this->readingBuffer.size() < boundary.size() + 4)
            return;

        if (this->readingBuffer.substr(0, boundary.size() + 2).toStr() !=  boundary + "\r\n")
            throw HttpError(BadRequest, "Bad Request");
        else
        {
            this->counter += boundary.size() + 2;
            this->readingBuffer = this->readingBuffer.substr(boundary.size() + 2, this->readingBuffer.size());
            this->state = ContentDisposition;
        }
    }
    if (state == ContentDisposition)
    {
        std::string gigaStr;
        pos = this->readingBuffer.find("\r\n\r\n");
        // std::cout << "hello: " << this->readingBuffer.substr(pos, readingBuffer.size()).toStr() << std::endl;
        if (pos == std::string::npos)
            return;
        if (isContentDispositionValid(this->readingBuffer.substr(0, pos).toStr(), gigaStr))
        {
            if (fileExists((ParsingReq::upload_path + "/" + gigaStr).c_str()))
                throw HttpError(Conflict, "Conflict");
            else
                this->tmpFiles.push_back(ParsingReq::upload_path + "/" + gigaStr);
            this->counter += pos + 4;
            this->readingBuffer = this->readingBuffer.substr(pos + 4, this->readingBuffer.size());
            state = FileContent;
        }
        else
            throw HttpError(BadRequest, "Bad Request");
    }
    if (state == FileContent)
    {
      
        pos = this->readingBuffer.find("\r\n" + boundary);
        if (pos != std::string::npos)
        {
            // std::cout << "here 1\n";
            writeToFile(this->readingBuffer.substr(0, pos).toStr(), this->tmpFiles[this->tmpFiles.size() - 1]);
            this->counter += pos + 2;
            this->readingBuffer = this->readingBuffer.substr(pos + 2, this->readingBuffer.size());
            state = EndBound;
        }
        else
        {
            size_t length = this->readingBuffer.size();
            if (length > ("\r\n" + boundary).size() + 64 * BUFFER_SIZE)
            {
                writeToFile(this->readingBuffer.substr(0, length - ("\r\n" + boundary).size()).toStr(), this->tmpFiles[this->tmpFiles.size() - 1]);
                this->counter += length - ("\r\n" + boundary).size();
                this->readingBuffer = this->readingBuffer.substr(length - ("\r\n" + boundary).size(), this->readingBuffer.size());
            }
        }
    }
    if (state == EndBound)
    {
       
        if (this->readingBuffer.size() < boundary.size() + 4)
            return;
        if (this->readingBuffer.substr(0, boundary.size() + 4).toStr() == boundary + "--\r\n")
        {
            if (this->counter + boundary.size() + 4 > static_cast<size_t>(ParsingReq::serverConfig.maxBodySize))
                throw HttpError(PayloadTooLarge, "Payload Too Large");
            if (this->counter + boundary.size() + 4 > this->contentLength)
                throw HttpError(BadRequest, "Bad Request");
            this->tmpFiles.push_back(generateUniqueFileName());
            writeToFile(postHtmlResponseGenerator(tmpFiles), this->tmpFiles[this->tmpFiles.size() - 1]);
            setResponseParams("201", "Created", "Content-type: text/html\r\n", this->tmpFiles[this->tmpFiles.size() - 1]);
            // firstboundary = 1;
            tmpFiles.erase(tmpFiles.begin(), tmpFiles.end() - 1);
        }
        else
        {
            state = startBound;
            MutiplePartHandler();
        }
    }
        // std::cout << "counter: " << counter << ", max body size: " << RequestParser::ServerConfiguration.maxBodySize << ", contentLength: " << this->contentLength << std::endl;
    if (this->counter > ParsingReq::serverConfig.maxBodySize)
        throw HttpError(PayloadTooLarge, "Payload Too Large");
    if (this->counter > this->contentLength)
        throw HttpError(BadRequest, "Bad Request");
}

void ClientHandler::ChunkedHandler()
{
    // std::cout << "hi" << std::endl;
    if (!this->tmpFiles.size())
    {
        if (msg.headers.find("Content-Type") != msg.headers.end())
            this->tmpFiles.push_back(generateUniqueFileName(ParsingReq::upload_path, getExtensionPost(msg.headers["Content-Type"])));
        else
            this->tmpFiles.push_back(generateUniqueFileName(ParsingReq::upload_path, ".tmp"));
    }
    if (!chunkSize)
    {
        if (readingBuffer.find("\r\n") != std::string::npos)
        {
            // std::cout << readingBuffer.substr(0, readingBuffer.find("\r\n")).toStr() << "|" << std::endl;
            if (!isValidBase(readingBuffer.substr(0, readingBuffer.find("\r\n")).toStr(), chunkSize, 16))
                throw HttpError(BadRequest, "Bad Request");
            // std::cout << chunkSize << " " << readingBuffer.substr(0, readingBuffer.find("\r\n")).toStr() << std::endl;
            if (!chunkSize)
            {
                if (readingBuffer.find("\r\n\r\n") != std::string::npos)
                {
                    // std::cout << "start chunked" << std::endl;
                    writeToFile(this->readingBuffer.substr(0, chunkSize).toStr(), this->tmpFiles[0]);
                    if (iscgi)
                    {
                        execCGI();
                        return;
                    }
                    this->tmpFiles.push_back(generateUniqueFileName());
                    writeToFile(postHtmlResponseGenerator(tmpFiles), this->tmpFiles[this->tmpFiles.size() - 1]);
                    setResponseParams("201", "Created", "Content-type: text/html\r\n", this->tmpFiles[this->tmpFiles.size() - 1]);
                    tmpFiles.erase(tmpFiles.begin(), tmpFiles.end() - 1);
                    // std::cout << "end chunked" << std::endl;
                }
                return;
            }
            else
            {
                counter += readingBuffer.find("\r\n") + 2;
                readingBuffer = readingBuffer.substr(readingBuffer.find("\r\n") + 2, readingBuffer.size());
            }
        }
        else
            return;
    }
    // std::cout << "==|" <<replaceNewlineWithLiteral(this->readingBuffer.toStr()) << "|==" << std::endl;
    if (this->readingBuffer.size() <= chunkSize + 1)
        return;
    // std::cout << "here: " << chunkSize << std::endl;
    // std::cout << "|" <<replaceNewlineWithLiteral(this->readingBuffer.substr(chunkSize, 2).toStr()) << "|" << std::endl;
    if (this->readingBuffer.substr(chunkSize, 2).toStr() != "\r\n")
        throw HttpError(BadRequest, "Bad Request");
    else
    {
        writeToFile(this->readingBuffer.substr(0, chunkSize).toStr(), this->tmpFiles[0]);
        this->readingBuffer = this->readingBuffer.substr(chunkSize + 2, this->readingBuffer.size());
        counter += chunkSize;
        chunkSize = 0;
        if (counter > ParsingReq::serverConfig.maxBodySize)
            throw HttpError(BadRequest, "Bad Request");
        ChunkedHandler();
    }
}

void ClientHandler::RegularDataHandler()
{
    // std::cout << "wtf 1" << std::endl;
    if (!this->tmpFiles.size())
    {
    // std::cout << "wtf 2" << std::endl;

        if (msg.headers.find("Content-Type") != msg.headers.end())
            this->tmpFiles.push_back(generateUniqueFileName(ParsingReq::upload_path, getExtensionPost(msg.headers["Content-Type"])));
        else
            this->tmpFiles.push_back(generateUniqueFileName(ParsingReq::upload_path, ".tmp"));

    }
    // std::cout << "wtf 3" << std::endl;

        // std::cout << replaceNewlineWithLiteral(this->readingBuffer.toStr()) << std::endl;
        writeToFile(this->readingBuffer.toStr(), this->tmpFiles[0]);
        counter += readingBuffer.size();
        // std::cout << counter << " " << contentLength << std::endl;
        this->readingBuffer.erase(0, this->readingBuffer.size());
        if (this->counter > ParsingReq::serverConfig.maxBodySize)
        throw HttpError(PayloadTooLarge, "Payload Too Large");
        if (this->counter > this->contentLength)
            throw HttpError(BadRequest, "Bad Request");
        if (counter == contentLength)
        {
            // std::cout << "hello" << std::endl;
            if (iscgi)
            {
                execCGI();
                return;
            }
            this->tmpFiles.push_back(generateUniqueFileName());
            writeToFile(postHtmlResponseGenerator(tmpFiles), this->tmpFiles[this->tmpFiles.size() - 1]);
            setResponseParams("201", "Created", "Content-type: text/html\r\n", this->tmpFiles[this->tmpFiles.size() - 1]);

            tmpFiles.erase(tmpFiles.begin(), tmpFiles.end() - 1);
        }
}



void ClientHandler::PostMethod()
{
    // std::cout << "================================" << std::endl;
    if (in)
        readFromSocket();
    else
        in = 1;

    if (msg.headers.find("Transfer-Encoding") != msg.headers.end())
    {
        // implement chunked
        ChunkedHandler();
    }
    else if (msg.headers.find("Content-Type") != msg.headers.end() && checkMultipart(msg.headers["Content-Type"], boundary) && !iscgi)
    {

        MutiplePartHandler();
    }
    else
    {
        // throw HttpError(BadRequest, "Bad Request");
        RegularDataHandler();
    }

}
