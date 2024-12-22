/* -- includes.hpp -- */

#pragma once

#ifndef INCLUDES_HPP
# define INCLUDES_HPP

/* ERROR MACROS */
#define SYNTAX_ERROR "Syntax error in configuration file."
#define DUPLICATE_ENTRY "Duplicate entry detected."
#define SERVER_ERROR "Expected 'server:' to start a new server block."
#define LOCATION_ERROR "Expected 'location:' to start a new location block."
#define SERVER_BLOCK_ERROR "Error in location block configuration."
/* SAFY 7BASS */

/* STANDARD LIBRARY HEADERS */
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <cctype>
#include <cstddef> /* for size_t, (...) */
/* SAFY 7BESS */


#endif /* ayeh, ayeh dakshi li frassek */