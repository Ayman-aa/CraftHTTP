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

extern key_value kv;

class ConfigurationParser : public ServerConfiguration
{
	public:
		static const ssize_t MAX_BODY_SIZE = 2147483648;
		/* init constructor */
		ConfigurationParser(string& filePath);
		/* end */
		
		~ConfigurationParser() {
			for (size_t i = 0; i < servers.size(); i++) 
				delete servers[i];
		}

		vector<ServerConfiguration*> servers;
		ServerConfiguration currentServer;
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

		bool extractErrorPages(ifstream& file, int& currentLineNumber);

		bool extractLocationInfos(ifstream& file, int& currentLineNumber, Location& location);
		bool servLocationLine(key_value& k_v, Location& location);

		bool extractAutoIndexValue(key_value& k_v, Location& location);

		bool extractAllowedMethods(key_value& k_v, Location& location);
		bool isValidMethod(const string& method);

		bool extractIndexValues(key_value& k_v, Location& location);
		bool isValidIndex(const string& index);

		bool extractReturnValue(key_value& k_v, Location& location);

		bool extractRootValue(key_value& k_v, Location& location);

		bool isValidCgiKey(const string& method);
		bool extractCgiPath(ifstream& file, Location& location, int& currentLineNumber);
};
#endif /* ayeh ayeh, configParser.hpp */
