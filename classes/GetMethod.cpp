#include "../includes/ClientHandler.hpp"

void ClientHandler::GetMethod(){
    if (isdir)
    {
        if (!loc.index.empty())
        {
            if (GetIndex())
                return ;
        }
        if (!loc.autoindex)
            throw HttpError(Forbidden, "Forbidden");
        else
            GetAutoIndex();
    }
    else{
        if (iscgi)
            execCGI();
        else
            setResponseParams("200", "OK", "", fullLocation); // pass file to send
    }
}

int ClientHandler::GetIndex(){
    std::vector<std::string>::iterator it = loc.index.begin();
    struct stat fileInfo;

    for (; it != loc.index.end(); it++)
    {
        std::string indexName = loc.root + loc.path +"/" + (*it);
        if (stat(indexName.c_str(), &fileInfo) == 0) {
            if (S_ISREG(fileInfo.st_mode)) {
                if (!access(indexName.c_str(), R_OK)) {
                    std::string fileExtension = getFileExtension(indexName);
                    if (loc.cgi_path.find(fileExtension) != loc.cgi_path.end()) {
                        cgi = loc.cgi_path[fileExtension];
                        fullLocation = indexName;
                        iscgi = true;
                        execCGI();
                    } else {
                        setResponseParams("200", "OK", "", indexName);
                    }
                    return 1;
                }
            }
        }
    }
    return 0;
}
void ClientHandler::GetAutoIndex(){
    std::string AIfile = generateUniqueFileName("/tmp", ".html");
    std::ofstream autoindexFile(AIfile.c_str());

    tmpFiles.push_back(AIfile);
    if (!autoindexFile.is_open())
        throw HttpError(InternalServerError, "Internal Server Error");
    
    DIR *dir = opendir(fullLocation.c_str());

    if (dir == NULL){
        autoindexFile.close();
        throw HttpError(InternalServerError, "Internal Server Error");
    }

    // html header
    autoindexFile << "<html>\n<head>\n<title>Autoindex</title>\n</head>\n<body>\n";
    autoindexFile << "<h1>Autoindex</h1>\n<ul>\n";

    // read directory
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL){
        if (entry->d_name[0] != '.'){
            std::string tmpName = msg.uri.path + "/";
            if (msg.uri.path == "/")
                tmpName.resize(1);
            // write a list item with a link to the file
            autoindexFile << "<li><a href=\"" << tmpName + entry->d_name << "\">" << entry->d_name << "</a></li>\n";
        }
    }

    // close html file
    autoindexFile << "</ul>\n</body>\n</html>";
    autoindexFile.close();
    setResponseParams("200", "OK", "", AIfile);
    closedir(dir);
}

void ClientHandler::redirect(){
    if (!access(fullLocation.c_str(), F_OK)) // if file exists, temporary redirect
        setResponseParams("302", "Found", "Location: " + loc.redirection_return + "\r\n", "");
    else
        setResponseParams("301", "Moved Permanently", "Location: " + loc.redirection_return + "\r\n", "");
}