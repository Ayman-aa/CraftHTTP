/* -- configParser.hpp -- */

#pragma once
#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "config.hpp"

struct key_value {
	string key;
	string value;
} kv;

class ConfigurationParser : public ServerConfiguration
{
	private: 
	public:
		/* init constructor */
		ConfigurationParser(string& filePath);
		/* end */

		void load(ifstream& file);
		int getIndentLevel(const string& line);
		bool isValidRootLevel(string& line);
		bool verifyLineFormat(string& line, int indentLevel);
		bool isValidSecondLevel(string& line, string& key, string& value);
		void syntaxError(int currentLineNumber);\

		/* getters Setters for key value struct */
};
#endif /* ayeh ayeh, configParser.hpp */
