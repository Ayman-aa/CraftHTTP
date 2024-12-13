/* -- configParser.hpp -- */

#pragma once
#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include <cstddef> /* for size_t, ... */

#include "config.hpp"

struct key_value {
	string key;
	string value;
	string currentParsedValue;
	bool isCommentOrEmptyLine;
};

class ConfigurationParser : public ServerConfiguration
{
	public:
		static const ssize_t MAX_BODY_SIZE = 2147483648;
		/* init constructor */
		ConfigurationParser(string& filePath);
		/* end */

		void load(ifstream& file);
		int getIndentLevel(const string& line);
		bool isValidRootLevel(string& line);
		bool verifyLineFormat(string& line, int indentLevel);
		bool isValidSecondLevel(string& line);
		void syntaxError(int currentLineNumber);

		void clear_kv(key_value& kv);

		bool LineIsCommentOrEmpty(string& line);

		bool extractHostKey(key_value& k_v);
		bool isValidIPSegment(const string& segment);

		bool extractPortValue(key_value& k_v);
		bool isValidPortSegment(const string& segment);

		bool extractServerNamesValue(key_value& k_v);

		bool extractClientMaxBodySizeValue(key_value& k_v);

		bool extractErrorPages(fstream& file);

		
};
#endif /* ayeh ayeh, configParser.hpp */
