/* -- yaml.hpp -- */

#pragma once

#define MAX_CHARS_IN_LINE 512

#include <vector>
#include <sstream>
#include <stdexcept>
#include <unistd.h> /* close(), ... */
#include <string>
#include <fstream> /* for file handling */
#include "SmartPtr.hpp"
#include "CustomMap.hpp"

enum EntryType {
	INVALID,
    KEY_VALUE,       // Requires value
    CONTAINER        // Prepares for children
};

struct LineValidationResult {
    bool isValid;
    EntryType type; /* return {false, CONTAINER};  */
};

namespace YAML {
	class Node {
		private:
			std::string m_identifier; /* Nodes key */
			/* int isChild; */
			void *m_data;
			CustomMap m_children;	/* */
		public:
			/* best explanation found on reddit of AKostur:
			 *"Explicit" (in the context of the constructors) basically says 
			 *"you have to have the exact right type to use this function/constructor". 
			 *In the case of the copy and move constructors, by definition you had to 
			 *have been passing them the right type anyway. 
			 *Otherwise you'd be invoking some other constructor and not a copy. 
			 */
			explicit Node(const std::string& identifier, void* data = NULL); /* Constructor */
			
			/* Node manipulation */
			bool append(Node* node); /* Add child Node */
			Node* getChild(const std::string& identifier);
			std::string getID() const;

			/* Data Handling */
			template <typename T>
			T* getData() {
				return static_cast<T*>(m_data);
			}
			void setData(void *data);
			

	};
	class yaml{
		private:
			Node* m_root; /* Root of YAML document */
		public:
			/* constructor */
			yaml(const std::string& filePath);
			~yaml();
			/* is an valide root level line format, and other levels */
			bool isValidRootLevel(const std::string& line);
			LineValidationResult isValidSecondLevel(const std::string& line);
			/* simple line syntax check */
			bool verifyLineFormat(const std::string& line, int indentLevel);
			/* calculate indentation of the current line */
			int getIndentLevel(const std::string& line);
			/* file parsing */
			void load(const std::string& filePath);
			/* Node access */
			Node* getRootNode() {
				return m_root;
			}
	};

}
