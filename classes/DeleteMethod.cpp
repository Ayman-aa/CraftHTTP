#include <cstdio>
#include <cstring>
#include "../includes/ClientHandler.hpp"

void ClientHandler::DeleteMethod(){
    try {
        if (std::remove(fullLocation.c_str()) != 0) 
            throw HttpError(InternalServerError, "Internal Server Error");
            
        setResponseParams("200", "OK", "", "errorPages/DEL.html");
    } catch (const HttpError& e) {
        throw;
    }
}