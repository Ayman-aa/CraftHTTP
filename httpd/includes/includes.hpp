/* -- includes.hpp -- */

// #pragma once

#ifndef INCLUDES_HPP
# define INCLUDES_HPP


/* ========================== MACRO DEFINITIONS ========================== */
#define CHECK_DUPLICATE(FIELD) \
	if (!FIELD.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY) \

#define CHECK_AND_EXTRACT(FIELD, CONTAINER, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		CHECK_DUPLICATE(CONTAINER); \
		if (!verifyLineFormat(line, currentIndentLevel) || !FUNC(kv)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		continue ; \
	} \

#define CHECK_AND_EXTRACT_MAX_BODY_SIZE(FIELD, CONTAINER, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		if (CONTAINER != -1) syntaxError(currentLineNumber, DUPLICATE_ENTRY); \
		if (!verifyLineFormat(line, currentIndentLevel) || !FUNC(kv)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		continue ; \
	} \

#define CHECK_AND_EXTRACT_LOCATION(FIELD, CONTAINER, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		CHECK_DUPLICATE(CONTAINER); \
		if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		findLoc = true; \
		continue ; \
	} \

#define CHECK_AND_EXTRACT_LOCATION_BOOL(FIELD, FUNC) \
	if (line.find(FIELD) != string::npos) { \
		if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
			syntaxError(currentLineNumber, SYNTAX_ERROR); \
		findLoc = true; \
		continue ; \
	} \

#define HANDLE_LOCATION() \
    Location location; \
    if (!verifyLineFormat(line, currentIndentLevel)) \
        syntaxError(currentLineNumber, SYNTAX_ERROR); \
    if (!servLocationLine(kv, location)) \
        syntaxError(currentLineNumber, LOCATION_ERROR); \
    if (!extractLocationInfos(file, currentLineNumber, location)) \
        syntaxError(currentLineNumber, SERVER_BLOCK_ERROR); \
    currentServer.locations[location.path] = location; \

#define CHECK_STRING_FORMAT(STR, WHAT) \
	if (STR.find(WHAT) != string::npos) return false; \

#define VALIDATE_KV(KEY) \
	if (k_v.value.empty() || k_v.key != KEY) return false; \

#define CHECK_INVALID_CHARS(STR) \
    if (STR.find('&') != string::npos || \
        STR.find('|') != string::npos || \
        STR.find(';') != string::npos || \
        STR.find('$') != string::npos) return false; \

#define CHECK_ALL_DIGITS(STR) \
    for (size_t i = 0; i < STR.length(); i++) \
        if (!isdigit(STR[i])) return false; \

#define VALIDATE_NUMERIC_RANGE(VALUE, MIN, MAX) \
    ssize_t value = 0; \
    istringstream iss(VALUE); \
    iss >> value; \
    if (value < MIN || value > MAX) return false; \


/* ========================== ERROR MESSAGES =========================== */
#define SYNTAX_ERROR "Syntax error in configuration file."
#define DUPLICATE_ENTRY "Duplicate entry detected."
#define SERVER_ERROR "Expected 'server:' to start a new server block."
#define LOCATION_ERROR "Expected 'location:' to start a new location block."
#define SERVER_BLOCK_ERROR "Error in location block configuration."
/* SAFY 7BASS */

/* ========================== STANDARD LIBRARY ========================= */
#include <iostream>
#include <ostream>
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
#include <cstddef> /* bnisba l size_t, (...) */
#include <sys/epoll.h>
#include <cerrno>
/* SAFY 7BESS */


#endif /* ayeh, ayeh dakshi li frassek */
