/* -- configParser.hpp -- */

#pragma once
#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "config.hpp"

struct key_value {
	string key;
	string value;
	string currentParsedValue;
};

class ConfigurationParser : public ServerConfiguration
{
	public:
		/* init constructor */
		ConfigurationParser(string& filePath);
		/* end */

		void load(ifstream& file);
		int getIndentLevel(const string& line);
		bool isValidRootLevel(string& line);
		bool verifyLineFormat(string& line, int indentLevel);
		bool isValidSecondLevel(string& line);
		void syntaxError(int currentLineNumber);

		bool extractHostKey(key_value& k_v);
		bool isValidIPSegment(const string& segment);

		bool extractPortValue(key_value& k_v);
		bool isValidPortSegment(const string& segment);

		bool extractServerNamesValue(key_value& k_v);
};
#endif /* ayeh ayeh, configParser.hpp */
