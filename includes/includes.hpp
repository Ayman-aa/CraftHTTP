/* -- includes.hpp -- */

#ifndef INCLUDES_HPP
# define INCLUDES_HPP


/* ========================== MACRO DEFINITIONS ========================== */
/**
 * @brief Checks if the specified field is non-empty.
 *
 * If the FIELD is not empty, a syntax error is raised indicating a duplicate entry.
 *
 * @param FIELD The field to be checked.
 */
#define CHECK_DUPLICATE(FIELD) \
    if (!FIELD.empty()) syntaxError(currentLineNumber, DUPLICATE_ENTRY) \

/**
 * @brief Checks for the presence of a FIELD in a configuration line and extracts its value.
 *
 * If the FIELD is found, verifies line format and extracts the value using FUNC.
 * Raises syntax error on failure.
 *
 * @param FIELD The configuration key to search for.
 * @param CONTAINER The container holding the configuration value.
 * @param FUNC The function used to extract the value.
 */
#define CHECK_AND_EXTRACT(FIELD, CONTAINER, FUNC) \
    if (line.find(FIELD) !=std::string::npos) { \
        CHECK_DUPLICATE(CONTAINER); \
        if (!verifyLineFormat(line, currentIndentLevel) || !FUNC(kv)) \
            syntaxError(currentLineNumber, SYNTAX_ERROR); \
        continue ; \
    } \

/**
 * @brief Checks and extracts a maximum body size field from a configuration line.
 *
 * Validates that the field has not been set already and extracts its value.
 * Raises syntax error if duplicate entry or extraction failure is detected.
 *
 * @param FIELD The configuration key to search for.
 * @param CONTAINER The container holding the numeric value.
 * @param FUNC The function used to extract the value.
 */
#define CHECK_AND_EXTRACT_MAX_BODY_SIZE(FIELD, CONTAINER, FUNC) \
    if (line.find(FIELD) !=std::string::npos) { \
        if (CONTAINER != -1) syntaxError(currentLineNumber, DUPLICATE_ENTRY); \
        if (!verifyLineFormat(line, currentIndentLevel) || !FUNC(kv)) \
            syntaxError(currentLineNumber, SYNTAX_ERROR); \
        continue ; \
    } \

/**
 * @brief Checks for location configuration and extracts its value.
 *
 * Verifies line format and uses FUNC to extract location details.
 * Sets the findLoc flag on a successful extraction.
 *
 * @param FIELD The configuration key for location.
 * @param CONTAINER The container holding the location value.
 * @param FUNC The function used to extract location information.
 */
#define CHECK_AND_EXTRACT_LOCATION(FIELD, CONTAINER, FUNC) \
    if (line.find(FIELD) !=std::string::npos) { \
        CHECK_DUPLICATE(CONTAINER); \
        if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
            syntaxError(currentLineNumber, SYNTAX_ERROR); \
        findLoc = true; \
        continue ; \
    } \

/**
 * @brief Checks for location configuration without enforcing duplicate checks.
 *
 * Verifies the format and extracts location data using FUNC.
 * Sets the findLoc flag upon a successful extraction.
 *
 * @param FIELD The configuration key for location.
 * @param FUNC The function used for extracting location information.
 */
#define CHECK_AND_EXTRACT_LOCATION_BOOL(FIELD, FUNC) \
    if (line.find(FIELD) !=std::string::npos) { \
        if (!verifyLineFormat(line, 1) || !FUNC(kv, location)) \
            syntaxError(currentLineNumber, SYNTAX_ERROR); \
        findLoc = true; \
        continue ; \
    } \

/**
 * @brief Handles location configuration by extracting and validating its properties.
 *
 * Creates a Location object, verifies the line format, extracts location details,
 * and assigns the location to the current server configuration.
 */
#define HANDLE_LOCATION() \
    Location location; \
    if (!verifyLineFormat(line, currentIndentLevel)) \
        syntaxError(currentLineNumber, SYNTAX_ERROR); \
    if (!servLocationLine(kv, location)) \
        syntaxError(currentLineNumber, LOCATION_ERROR); \
    if (!extractLocationInfos(file, currentLineNumber, location)) \
        syntaxError(currentLineNumber, SERVER_BLOCK_ERROR); \
    currentServer.locations[location.path] = location; \

/**
 * @brief Validates that a string does not contain a specific substring.
 *
 * Returns false if WHAT is found within STR.
 *
 * @param STR The string to be checked.
 * @param WHAT The substring which must not be present.
 */
#define CHECK_STRING_FORMAT(STR, WHAT) \
    if (STR.find(WHAT) !=std::string::npos) return false; \

/**
 * @brief Validates that the provided key_value object has a matching key with a non-empty value.
 *
 * @param KEY The expected key.
 */
#define VALIDATE_KV(KEY) \
    if (k_v.value.empty() || k_v.key != KEY) return false; \

/**
 * @brief Checks that the string does not contain any invalid characters.
 *
 * Invalid characters include '&', '|', ';', and '$'.
 *
 * @param STR The string to be validated.
 */
#define CHECK_INVALID_CHARS(STR) \
    if (STR.find('&') !=std::string::npos || \
        STR.find('|') !=std::string::npos || \
        STR.find(';') !=std::string::npos || \
        STR.find('$') !=std::string::npos) return false; \

/**
 * @brief Verifies that all characters in a string are digits.
 *
 * Iterates over each character and returns false if a non-digit is found.
 *
 * @param STR The string to be checked.
 */
#define CHECK_ALL_DIGITS(STR) \
    for (size_t i = 0; i < STR.length(); i++) \
        if (!isdigit(STR[i])) return false; \

/**
 * @brief Validates that a numeric value falls within the specified range.
 *
 * Converts the string VALUE into a numeric type and checks that it is between MIN and MAX.
 *
 * @param VALUE The numeric string value.
 * @param MIN The minimum acceptable value.
 * @param MAX The maximum acceptable value.
 */
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

/* ========================== STANDARD LIBRARY ========================= */
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
#include <cstddef> /* bnisba l size_t, (...) */
/* SAFY 7BESS */


#endif /* ayeh, ayeh dakshi li frassek */