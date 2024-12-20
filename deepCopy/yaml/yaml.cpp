/* -- yaml.cpp -- */

#include "yaml.hpp"
#include <iostream>
using namespace std;

namespace YAML {
	/* Node class constructor */
	Node::Node(const string& identifier, void* data): m_identifier(identifier) {
		if (data != NULL)
			m_data = data;
		else
			m_data = NULL;
	}
	/* Node manipulation */
	bool Node::append(Node* node) {
		if (this->m_children.find(getID()) == NULL)
			return false;
		m_children.insert(node->getID(), node);
		return true;
	}
	Node* Node::getChild(const string& identifier) {
		return m_children.find(identifier);
	}
	string Node::getID() const {
		return m_identifier;
	}
	void Node::setData(void *data) {
		this->m_data = data;
	}

	/* yaml class */
	yaml::yaml(const string& filePath) {
		try {
			load(filePath);
		}
		catch(exception& e) {
			cout << e.what() << endl;
		}
	}
	yaml::~yaml(){}
	
	bool yaml::isValidRootLevel(const string& line) {
		return line == "server:";
	}

	LineValidationResult yaml::isValidSecondLevel(const string& line) {
		/* nbdaw with num of : */
		size_t colonCount = 0;
		for (size_t i = 0; i < line.length(); ++i) {
			if (line[i] == ':')
				colonCount++;
		}
		if (colonCount != 1) return (LineValidationResult){false, INVALID};
		
		/* na5do pos of colona */
		size_t colonPosition = line.find(':');
		/* nshofo l key */
		string key = line.substr(0, colonPosition);
		if (key.empty() || key.find(' ') != string::npos || key.find('\t') != string::npos)
			return (LineValidationResult){false, INVALID};
		/* daba value dial dak key */
		string value = line.substr(colonPosition+1);
		size_t firstNoSpace = value.find_first_not_of(" \t");
		size_t lastNoSpace = value.find_last_not_of(" \t");
		if (value.empty() || firstNoSpace == string::npos) return (LineValidationResult){true, CONTAINER};
		if (firstNoSpace != string::npos && lastNoSpace  != string::npos)
			value = value.substr(firstNoSpace, lastNoSpace - firstNoSpace + 1);
		cout << "key: " << key << " | value: " << value<< endl;
		return (LineValidationResult){true, KEY_VALUE}; 
	}

	bool yaml::verifyLineFormat(const string& line, int indentLevel) {
		/*  Mzl ma3reftsh wesh empty line fih shi 7ma9 */
		if (line.empty())
			return true;
		size_t firstNoTab = line.find_first_not_of('\t');
		if (firstNoTab == string::npos)
			return true;
		if (!indentLevel) {		/* bime3na indentLevel == 0 */
			return isValidRootLevel(line);
		}
		string trimmedLine = line.substr(firstNoTab);
		/* cout << "trimmedLine: " << trimmedLine << endl; */
		if (indentLevel == 1) {
			LineValidationResult lvr = isValidSecondLevel(trimmedLine);
			return lvr.isValid;
		}
		return true;
	}

	/**
	* @brief Calculates the level of indentation for the current line in the YAML file.
 	* 
 	* This function determines the number of leading spaces or tabs in the given line,
	* which indicates the hierarchical level of the current node in the YAML structure.
	*
	* @param line The current line of the YAML file as a string.
 	* @return int The indentation level (number of leading tabs).
 	*/
	int yaml::getIndentLevel(const string& line) {
		int IndentLevel = 0; /* must be tabs nor space */
		char tab = '\t';
		
		for (size_t i = 0; i < line.length(); i++) {
			if (line[i] == tab)
				IndentLevel++;
			else 
				break;
		}
		return IndentLevel;
	}

	void yaml::load(const string& filePath) {
		string line;
		int currentLineNumber = 0;
		int currentIndentLevel;
		/* file parsing
		 * TO-DO:
		 * 1. Open file.
		 * 2. Read line by line.
		 * 3. Handle indentation.
		 * 4. Create node hierarchy.
		 * 5. Handle different YAML structures.
		 */
		ifstream file(filePath.c_str());
		if (!file.is_open()) {
			throw runtime_error("failed to open file: " + filePath);
		}
		while (getline(file, line)) {
			currentLineNumber++;
			if (line.empty())
				continue ;
			currentIndentLevel = yaml::getIndentLevel(line);
			if (!yaml::verifyLineFormat(line, currentIndentLevel)) {
				ostringstream oss;
				oss << currentLineNumber;
				throw runtime_error("error: syntax error at line " + oss.str());
			}
		}
		file.close();	
	}
}
