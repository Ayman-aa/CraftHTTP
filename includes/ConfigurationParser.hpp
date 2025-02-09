#pragma once
#ifndef CONFIGURATIONPARSER_HPP
#define CONFIGURATIONPARSER_HPP

#include "ServerConfiguration.hpp"

/* ========================== STRUCTURES =============================== */

/**
 * @struct key_value
 * @brief Represents a key-value pair used during configuration parsing.
 */
struct key_value {
    std::string key;    /**< Configuration key */
    std::string value;  /**< Configuration value */
};

extern key_value kv;

/* ========================== MAIN CLASS =============================== */

/**
 * @class ConfigurationParser
 * @brief Parses configuration files and generates server configuration objects.
 *
 * The ConfigurationParser class extends ServerConfiguration to provide functionalities for
 * reading, validating, and storing configuration details from a file. It supports extraction of host,
 * port, error page settings as well as location, CGI, and upload configurations.
 */
class ConfigurationParser : public ServerConfiguration
{
    public:
        /* --------------- Constants & Constructor ----------------- */
        
        /**
         * @brief Maximum allowed client body size.
         */
        static const ssize_t maxBodySize = 2147483648;

		/**
         * @brief Checks if the given file path ends with .yaml extension.
         *
         * @param filePath Reference to the path of the configuration file.
         * @return true if file ends with .yaml, false otherwise.
         */
        bool yaml(std::string& filePath);

        /**
         * @brief Constructs a ConfigurationParser with the specified configuration file path.
         *
         * @param filePath Reference to the path of the configuration file.
         */
        ConfigurationParser(std::string& filePath);

        /**
         * @brief Default constructor.
         */
        ConfigurationParser() {}

        /**
         * @brief Destructor cleans up dynamically allocated server configurations.
         */
        ~ConfigurationParser() {
            for (size_t i = 0; i < servers.size(); i++) 
                delete servers[i];
        }

        /* --------------- Main Config Data ----------------- */
        
        /**
         * @brief List of server configurations obtained from the configuration file.
         */
        std::vector<ServerConfiguration*>	servers;

        /**
         * @brief The current server configuration being processed.
         */
        ServerConfiguration currentServer;

        /**
         * @brief Retrieves a server configuration that matches the specified host, ports, and server name.
         *
         * @param host The server host.
         * @param ports A vector of ports on which the server operates.
         * @param serverName The name of the server.
         * @return Pointer to the matching ServerConfiguration or nullptr if none match.
         */
        ServerConfiguration *getServerConfig(const std::string &host, const std::vector<std::string> &ports, const std::string &serverName);

        /* --------------- Parsing Functions ----------------- */
        
        /**
         * @brief Loads and parses the configuration file.
         *
         * @param file Input file stream of the configuration file.
         */
        void	load(std::ifstream& file);

        /**
         * @brief Computes the indentation level of a given line.
         *
         * @param line The configuration file line.
         * @return The indentation level.
         */
        int		getIndentLevel(const std::string& line);

        /**
         * @brief Validates if the given line is at the root configuration level.
         *
         * @param line The configuration file line.
         * @return True if the line is valid at the root level, false otherwise.
         */
        bool	isValidRootLevel(std::string& line);

        /**
         * @brief Verifies that a configuration line follows the expected formatting.
         *
         * @param line The configuration file line.
         * @param indentLevel The expected indentation level.
         * @return True if the format is as expected, false otherwise.
         */
        bool	verifyLineFormat(std::string& line, int indentLevel);

        /**
         * @brief Checks if a non-root level line adheres to the expected second-level formatting.
         *
         * @param line The configuration file line.
         * @return True if valid, false otherwise.
         */
        bool	isValidSecondLevel(std::string& line);

        /**
         * @brief Validates the integrity of a server configuration.
         *
         * @param server The server configuration to validate.
         * @param lineNumber The line number where the server configuration is defined.
         */
        void	validateServerConfiguration(ServerConfiguration &server, int lineNumber);
        
        /* --------------- Utility Functions ----------------------- */
        
        /**
         * @brief Reports a syntax error encountered during parsing.
         *
         * @param currentLineNumber The line number of the error.
         * @param errorMessage The error message to report.
         */
        void	syntaxError(int currentLineNumber, const std::string& errorMessage);

        /**
         * @brief Resets the key_value structure.
         *
         * @param kv Reference to the key_value structure to clear.
         */
        void	clear_kv(key_value& kv);

        /**
         * @brief Determines if a line is a comment or empty.
         *
         * @param line The configuration file line.
         * @return True if the line is a comment or empty, false otherwise.
         */
        bool	LineIsCommentOrEmpty(std::string& line);

        /* --------------- Host & Port Processing ----------------- */
        
        /**
         * @brief Extracts and validates the host key from the configuration.
         *
         * @param k_v The key_value pair containing the host key.
         * @return True if extraction is successful, false otherwise.
         */
        bool	extractHostKey(key_value& k_v);

        /**
         * @brief Validates that a segment of an IP address is in the correct format.
         *
         * @param segment A segment of an IP address.
         * @return True if valid, false otherwise.
         */
        bool	isValidIPSegment(const std::string& segment);

        /**
         * @brief Extracts and validates the port value from the configuration.
         *
         * @param k_v The key_value pair containing the port information.
         * @return True if successful, false otherwise.
         */
        bool	extractPortValue(key_value& k_v);

        /**
         * @brief Validates that a segment of a port is numeric and correctly formatted.
         *
         * @param segment A segment of the port.
         * @return True if valid, false otherwise.
         */
        bool	isValidPortSegment(const std::string& segment);
    
        /* --------------- Server Configuration ------------------- */
        
        /**
         * @brief Extracts the server names from the configuration.
         *
         * @param k_v The key_value pair containing the server names.
         * @return True if extraction is successful, false otherwise.
         */
        bool	extractServerNamesValue(key_value& k_v);

        /**
         * @brief Extracts the client maximum body size value from the configuration.
         *
         * @param k_v The key_value pair containing the client max body size.
         * @return True if extraction and validation are successful, false otherwise.
         */
        bool	extractClientMaxBodySizeValue(key_value& k_v);

        /**
         * @brief Extracts error pages from the configuration file.
         *
         * @param file Input file stream.
         * @param currentLineNumber The current line number in the configuration file.
         * @return True if error pages are successfully extracted, false otherwise.
         */
        bool	extractErrorPages(std::ifstream& file, int& currentLineNumber);

        /* --------------- Location Configuration ---------------- */
        
        /**
         * @brief Extracts location information from the configuration.
         *
         * @param file Input file stream.
         * @param currentLineNumber The current line number.
         * @param location The Location object to populate.
         * @return True if successful, false otherwise.
         */
        bool	extractLocationInfos(std::ifstream& file, int& currentLineNumber, Location& location);

        /**
         * @brief Processes a location line and populates the Location structure.
         *
         * @param k_v The key_value pair containing location data.
         * @param location The Location object to populate.
         * @return True if successful, false otherwise.
         */
        bool	servLocationLine(key_value& k_v, Location& location);

        /**
         * @brief Extracts the autoindex value for a location.
         *
         * @param k_v The key_value pair for autoindex.
         * @param location The Location object to update.
         * @return True if successful, false otherwise.
         */
        bool	extractAutoIndexValue(key_value& k_v, Location& location);

        /**
         * @brief Extracts the allowed HTTP methods for a location.
         *
         * @param k_v The key_value pair containing allowed methods.
         * @param location The Location object to update.
         * @return True if successful, false otherwise.
         */
        bool	extractAllowedMethods(key_value& k_v, Location& location);

        /**
         * @brief Validates that the provided HTTP method is supported.
         *
         * @param method The HTTP method to validate.
         * @return True if valid, false otherwise.
         */
        bool	isValidMethod(const std::string& method);

        /**
         * @brief Extracts index values from the configuration for a location.
         *
         * @param k_v The key_value pair containing index data.
         * @param location The Location object to update.
         * @return True if successful, false otherwise.
         */
        bool	extractIndexValues(key_value& k_v, Location& location);

        /**
         * @brief Validates that an index value is acceptable.
         *
         * @param index The index value to check.
         * @return True if valid, false otherwise.
         */
        bool	isValidIndex(const std::string& index);

        /**
         * @brief Extracts return values for a location configuration.
         *
         * @param k_v The key_value pair containing the return value.
         * @param location The Location object to update.
         * @return True if successful, false otherwise.
         */
        bool	extractReturnValue(key_value& k_v, Location& location);

        /**
         * @brief Extracts the root configuration for a location.
         *
         * @param k_v The key_value pair containing the root information.
         * @param location The Location object to update.
         * @return True if successful, false otherwise.
         */
        bool	extractRootValue(key_value& k_v, Location& location);

        /* --------------- CGI Configuration -------------------- */
        
        /**
         * @brief Validates that the CGI key is supported.
         *
         * @param method The CGI method key.
         * @return True if valid, false otherwise.
         */
        bool	isValidCgiKey(const std::string& method);

        /**
         * @brief Extracts the CGI path from the configuration file.
         *
         * @param file Input file stream.
         * @param location The Location object to update.
         * @param currentLineNumber The current line number.
         * @return True if extraction is successful, false otherwise.
         */
        bool	extractCgiPath(std::ifstream& file, Location& location, int& currentLineNumber);

        /**
         * @brief Extracts the upload path for a location.
         *
         * @param k_v The key_value pair containing the upload path.
         * @param location The Location object to update.
         * @return True if successful, false otherwise.
         */
        bool	extractUploadPath(key_value& k_v, Location& location);

        /* ---------- Helper method, bash nrje3 offset ---------- */
        
        /**
         * @brief Seeks to a specific offset in the configuration file.
         *
         * @param file Input file stream.
         * @param line The current configuration line.
         * @param currentLineNumber The current line number.
         * @param retVal The return value upon success.
         * @return True if successful, false otherwise.
         */
        bool	FileSeekg(std::ifstream& file, const std::string& line, int &currentLineNumber, bool retVal);
};
typedef ConfigurationParser Configurations;
#endif /* ayeh ayeh, ConfigurationParser.hpp */
