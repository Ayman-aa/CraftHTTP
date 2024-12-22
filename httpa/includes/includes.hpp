/* -- includes.hpp -- */

#pragma once

#ifndef INCLUDES_HPP
# define INCLUDES_HPP

/* 
  * Hadchi kaykhlik tsta3mel les fonctions ou les classes dyal std (bhal cout, string) 
  * bla ma tzid 'std::'.  Mzyan f les petits projets, walakin f les grands projets yqder
  *  ydir clash m3a les noms dyal les variables dyalk. Ila bghiti code mtnaddem w professional,
  *  khassak tsta3mel 'std::' directement.
*/
using namespace std;


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