/* -- ConfigurationParser.hpp -- */

#pragma once
#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "ServerConfiguration.hpp"


/* ========================== STRUCTURES =============================== */
struct key_value {
	string key;
	string value;
};

extern key_value kv;

/* ========================== MAIN CLASS =============================== */
class ConfigurationParser : public ServerConfiguration
{
	public:
		/* --------------- Constants & Constructor ----------------- */
		static const ssize_t maxBodySize = 2147483648;

		ConfigurationParser(string& filePath);
		ConfigurationParser() {}
		~ConfigurationParser() {
			for (size_t i = 0; i < servers.size(); i++) 
				delete servers[i];
		}

		/* --------------- Main Config Data ----------------- */
		vector<ServerConfiguration*>	servers;
		ServerConfiguration				currentServer;
		ServerConfiguration &getServerConfig(const std::string &host,const std::vector<std::string> &ports, const std::string &serverName);

		/* --------------- Parsing Functions ----------------- */
		void	load(ifstream& file);
		int		getIndentLevel(const string& line);
		bool	isValidRootLevel(string& line);
		bool	verifyLineFormat(string& line, int indentLevel);
		bool	isValidSecondLevel(string& line);
		
		/* --------------- Utility Functions ----------------------- */
		void	syntaxError(int currentLineNumber, const string& errorMessage);
		void	clear_kv(key_value& kv);
		bool	LineIsCommentOrEmpty(string& line);

		/* --------------- Host & Port Processing ----------------- */
		bool	extractHostKey(key_value& k_v);
		bool	isValidIPSegment(const string& segment);
		bool	extractPortValue(key_value& k_v);
		bool	isValidPortSegment(const string& segment);
	
		/* --------------- Server Configuration ------------------- */
		bool	extractServerNamesValue(key_value& k_v);
		bool	extractClientMaxBodySizeValue(key_value& k_v);
		bool	extractErrorPages(ifstream& file, int& currentLineNumber);

		/* --------------- Location Configuration ---------------- */
		bool	extractLocationInfos(ifstream& file, int& currentLineNumber, Location& location);
		bool	servLocationLine(key_value& k_v, Location& location);
		bool	extractAutoIndexValue(key_value& k_v, Location& location);
		bool	extractAllowedMethods(key_value& k_v, Location& location);
		bool	isValidMethod(const string& method);
		bool	extractIndexValues(key_value& k_v, Location& location);
		bool	isValidIndex(const string& index);
		bool	extractReturnValue(key_value& k_v, Location& location);
		bool	extractRootValue(key_value& k_v, Location& location);

		/* --------------- CGI Configuration -------------------- */
		bool	isValidCgiKey(const string& method);
		bool	extractCgiPath(ifstream& file, Location& location, int& currentLineNumber);
		bool	extractUploadPath(key_value& k_v, Location& location);

		/* ---------- Helper method, bash nrje3 offset ---------- */
		bool	FileSeekg(ifstream& file, const string& line, int &currentLineNumber, bool retVal);
};
typedef ConfigurationParser Configurations;
#endif /* ayeh ayeh, ConfigurationParser.hpp */
