#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../includes/ClientHandler.hpp"

void deleteDirectory(const char* path)
{
    DIR* dir = opendir(path);
    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) 
                continue;
            
            std::string fullPath = std::string(path) + "/" + entry->d_name;
            struct stat statbuf;
            if (stat(fullPath.c_str(), &statbuf) == 0)
            {
                if (S_ISDIR(statbuf.st_mode)) 
                    deleteDirectory(fullPath.c_str());
                else 
                    remove(fullPath.c_str());
            }
        }
        closedir(dir);
        rmdir(path);
    }
    else
        remove(path);
}

void ClientHandler::DeleteMethod()
{
    try
    {
        struct stat statbuf;
        if (stat(fullLocation.c_str(), &statbuf) == 0)
        {
            if (S_ISDIR(statbuf.st_mode)) 
                deleteDirectory(fullLocation.c_str());
            else 
                remove(fullLocation.c_str());
    
            setResponseParams("200", "OK", "", "errorPages/DEL.html");
        }
        else 
            throw HttpError(NotFound, "Not Found");
    } catch (const HttpError& e)
    {
        throw;
    }
}