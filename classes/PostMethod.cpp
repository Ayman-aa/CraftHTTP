#include "../includes/ClientHandler.hpp"

void writeToFile(const std::string &data, const std::string& fileName)
{
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
    std::string bound = strtrim(str.substr(sep_pos + 1));
    sep_pos = bound.find("=");
    if (sep_pos == std::string::npos)
        return false;
    if (strtrim(bound.substr(0, sep_pos)) != "boundary")
        return false;
    boundary = strtrim(bound.substr(sep_pos + 1));
    if (boundary.empty())
        return false;
    if (boundary[0] == '"' && boundary[boundary.size() - 1] == '"')
        boundary = boundary.substr(1, boundary.size() - 2); // Remove quotes if present
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
        fileName = sanitizeFilename(fileName);
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
        if (pos == std::string::npos)
            return;

        if (isContentDispositionValid(this->readingBuffer.substr(0, pos).toStr(), gigaStr))
        {
            if (fileExists((RequestParser::upload_path + "/" + gigaStr).c_str()))
                throw HttpError(Conflict, "Conflict");
            else
                this->tmpFiles.push_back(RequestParser::upload_path + "/" + gigaStr);

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
            if (this->counter + boundary.size() + 4 > static_cast<size_t>(RequestParser::ServerConfig.maxBodySize))
                throw HttpError(PayloadTooLarge, "Payload Too Large");
            if (this->counter + boundary.size() + 4 > this->contentLength)
                throw HttpError(BadRequest, "Bad Request");
            this->tmpFiles.push_back(generateUniqueFileName());
            writeToFile(postHtmlResponseGenerator(tmpFiles), this->tmpFiles[this->tmpFiles.size() - 1]);
            setResponseParams("201", "Created", "Content-type: text/html\r\n", this->tmpFiles[this->tmpFiles.size() - 1]);
            tmpFiles.erase(tmpFiles.begin(), tmpFiles.end() - 1);
        }
        else
        {
            state = startBound;
            MutiplePartHandler();
        }
    }
    
    if (this->counter > RequestParser::ServerConfig.maxBodySize)
        throw HttpError(PayloadTooLarge, "Payload Too Large");

    if (this->counter > this->contentLength)
        throw HttpError(BadRequest, "Bad Request");
}

void ClientHandler::ChunkedHandler()
{
    if (!this->tmpFiles.size())
    {
        if (message.headers.find("Content-Type") != message.headers.end())
            this->tmpFiles.push_back(generateUniqueFileName(RequestParser::upload_path, getExtensionPost(message.headers["Content-Type"])));
        else
            this->tmpFiles.push_back(generateUniqueFileName(RequestParser::upload_path, ".tmp"));
    }

    while (true)
    {
        if (!chunkSize)
        {
            size_t pos = readingBuffer.find("\r\n");
            if (pos != std::string::npos)
            {
                if (!isValidBase(readingBuffer.substr(0, pos).toStr(), chunkSize, 16))
                    throw HttpError(BadRequest, "Bad Request");

                readingBuffer = readingBuffer.substr(pos + 2);
                if (chunkSize == 0)
                {
                    // End of chunks, process trailers
                    pos = readingBuffer.find("\r\n\r\n");
                    if (pos != std::string::npos)
                    {
                        readingBuffer = readingBuffer.substr(pos + 4);
                        if (isCGIfile)
                        {
                            execCGI();
                            return;
                        }
                        this->tmpFiles.push_back(generateUniqueFileName());
                        writeToFile(postHtmlResponseGenerator(tmpFiles), this->tmpFiles[this->tmpFiles.size() - 1]);
                        setResponseParams("201", "Created", "Content-type: text/html\r\n", this->tmpFiles[this->tmpFiles.size() - 1]);
                        tmpFiles.erase(tmpFiles.begin(), tmpFiles.end() - 1);
                        return;
                    }
                    else
                    {
                        return; // Wait for more data
                    }
                }
            }
            else
            {
                return; // Wait for more data
            }
        }

        if (readingBuffer.size() < chunkSize + 2)
            return; // Wait for more data

        if (readingBuffer.substr(chunkSize, 2).toStr() != "\r\n")
            throw HttpError(BadRequest, "Bad Request");

        writeToFile(readingBuffer.substr(0, chunkSize).toStr(), this->tmpFiles[0]);
        readingBuffer = readingBuffer.substr(chunkSize + 2);
        counter += chunkSize;
        chunkSize = 0;

        if (counter > RequestParser::ServerConfig.maxBodySize)
            throw HttpError(PayloadTooLarge, "Payload Too Large");
    }
}

void ClientHandler::RegularDataHandler()
{
    if (!this->tmpFiles.size())
    {

        if (message.headers.find("Content-Type") != message.headers.end())
            this->tmpFiles.push_back(generateUniqueFileName(RequestParser::upload_path, getExtensionPost(message.headers["Content-Type"])));
        else
            this->tmpFiles.push_back(generateUniqueFileName(RequestParser::upload_path, ".tmp"));
    }
    writeToFile(this->readingBuffer.toStr(), this->tmpFiles[0]);
    counter += readingBuffer.size();
    this->readingBuffer.erase(0, this->readingBuffer.size());

    if (this->counter > RequestParser::ServerConfig.maxBodySize)
        throw HttpError(PayloadTooLarge, "Payload Too Large");

    if (this->counter > this->contentLength)
        throw HttpError(BadRequest, "Bad Request");
        
    if (counter == contentLength)
    {
        if (isCGIfile)
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
    if (in)
        readFromSocket();
    else
        in = 1;

    if (message.headers.find("Transfer-Encoding") != message.headers.end())
        ChunkedHandler();
    else if (message.headers.find("Content-Type") != message.headers.end() && checkMultipart(message.headers["Content-Type"], boundary) && !isCGIfile)
        MutiplePartHandler();
    else
        RegularDataHandler();
}
