#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <map>
#include <vector>
#include <stdlib.h>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits.h>

/**
 * Trims the specified characters from both ends of the input string.
 *
 * @param str The string to trim.
 * @param charsToTrim A string containing the characters to remove from both ends.
 * @return The trimmed string.
 */
std::string strtrim(const std::string& str, const std::string& charsToTrim = " \t\n\r");

/**
 * Splits the input string using multiple delimiter strings.
 *
 * Searches for the earliest occurrence among the delimiters and splits
 * the string accordingly.
 *
 * @param str The string to split.
 * @param delimiters A vector of delimiter strings.
 * @return A vector of substrings after splitting.
 */
std::vector<std::string> splitWithDelimiters(const std::string& str, const std::vector<std::string>& delimiters);

/**
 * Splits a string by any of the specified delimiter characters.
 *
 * @param input The string to split.
 * @param delimiters A string containing delimiter characters.
 * @param includeEmpty Flag indicating whether to include empty substrings.
 * @return A vector containing the split substrings.
 */
std::vector<std::string> strSplit(const std::string& input, const std::string& delimiters, bool includeEmpty);

/**
 * Checks if all characters in the given string are within the allowed characters.
 *
 * @param str The string to validate.
 * @param allowedChars A string containing all allowed characters.
 * @return True if all characters in 'str' are found in 'allowedChars'; false otherwise.
 */
bool allCharactersAllowed(const std::string& str, const std::string& allowedChars);

/**
 * Converts an integer to its string representation.
 *
 * @param n The integer to convert.
 * @return A string representing the integer.
 */
std::string toString(int n);

/**
 * Generates a unique file name within the specified directory.
 *
 * The function repeatedly generates a random filename until a
 * non-existing one is found.
 *
 * @param directory The directory in which to create the file (default is "/tmp").
 * @param extension The file extension to append.
 * @return A unique file path.
 */
std::string generateUniqueFileName(const std::string& directory = "/tmp", const std::string& extension = "");

/**
 * Retrieves the file extension from a given file path.
 *
 * @param file_path The complete file path.
 * @return The file extension if present; otherwise, an empty string.
 */
std::string getFileExtension(const std::string& file_path);

/**
 * Sanitizes a filename by removing forbidden characters.
 *
 * Removes characters such as '/' and '\' from the filename.
 *
 * @param filename The filename to sanitize.
 * @return The sanitized filename.
 */
std::string sanitizeFilename(const std::string& filename);

/**
 * Validates whether a given string represents a valid number in a specified base.
 *
 * Attempts to convert the string to an unsigned integer and checks
 * for conversion errors.
 *
 * @param number The string representation of the number.
 * @param size Output parameter to store the converted number.
 * @param base The numerical base for conversion.
 * @return True if the string is valid in the specified base; false otherwise.
 */
bool isValidBase(const std::string& number, unsigned int &size, const int base);

/**
 * Generates an HTML response string based on provided file names.
 *
 * (Definition to be kept in tools.cpp)
 *
 * @param filesNames A vector containing file names.
 * @return A string containing the generated HTML response.
 */
std::string postHtmlResponseGenerator(std::vector<std::string>& filesNames);

/**
 * Checks if a file exists at the specified path.
 *
 * @param name The name or path of the file.
 * @return True if the file exists; false otherwise.
 */
bool fileExists(const std::string& name);

/**
 * Decodes a URL-encoded string.
 *
 * Converts percent-encoded characters into their literal form and replaces '+' with a space.
 *
 * @param input The URL-encoded string.
 * @return The decoded string.
 */
std::string urlDecode(const std::string& input);

#endif