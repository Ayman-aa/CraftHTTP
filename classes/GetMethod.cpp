#include "../includes/ClientHandler.hpp"

void ClientHandler::GetMethod(){
    if (isDir)
    {
        if (!location.index.empty()){ 
            if (GetIndex())
                return ;
        }
        if (!location.autoindex)
            throw HttpError(Forbidden, "Forbidden");
        else
            GetAutoIndex();
    }
    else{
        if (isCGIfile)
            execCGI();
        else
            setResponseParams("200", "OK", "", fullLocation); // pass file to send
    }
}

int ClientHandler::GetIndex(){
    std::vector<std::string>::iterator it = location.index.begin();
    struct stat fileInfo;

    for (;it != location.index.end(); it++)
    {
        std::string indexName = location.root+"/"+(*it);
        stat(indexName.c_str(), &fileInfo);
        if (S_ISREG(fileInfo.st_mode)){
            if (!access(indexName.c_str(), R_OK)){
                std::string fileExtention = getFileExtention(indexName);
	            if (location.cgi_path.find(fileExtention) != location.cgi_path.end()){
	            	CGIpath = location.cgi_path[fileExtention];
                    fullLocation = indexName;
		            isCGIfile = true;
                    execCGI();
	            }
                else
                    setResponseParams("200", "OK", "", indexName);
                return 1;
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
            std::string tmpName = message.uri.path + "/";
            if (message.uri.path == "/")
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
        setResponseParams("302", "Found", "Location: " + location.redirection_return + "\r\n", "");
    else
        setResponseParams("301", "Moved Permanently", "Location: " + location.redirection_return + "\r\n", "");
}