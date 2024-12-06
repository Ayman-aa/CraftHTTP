/* -- yaml.cpp -- */

#include "yaml.hpp"
#include <iostream>
using namespace std;

namespace YAML {
	/* Node class constructor */
	Node::Node(const std::string& identifier, void* data = NULL): m_identifier {
		if (data != NULL)
			m_data = SmartPtr<void>(data);
		else
			m_data = SmartPtr<void>(NULL);
	}
	/* Node manipulation */
	bool Node::append(Node* node) {
		if (this->m_children.find(getID()) == NULL)
			return false;
		m_children.insert(node->getID(), node);
		return true;
	}
	Node* Node::getChild(const std::string& identifier) {
		return m_children.find(identifier);
	}
	std::string Node::getID() {
		return identifier;
	}
	void Node::setData(void *data) {
		this->m_data = data;
	}

	/* yaml class */
	yaml::yaml(const std::string& filePath) {
		if (!load(filePath))
			throw std::runtime_error("Error: Yaml parser object failed during parsing the given file.");
	}
	void load(const std::string& filePath) {
		/* file parsing
		 * TO-DO:
		 * 1. Open file.
		 * 2. Read line by line.
		 * 3. Handle indentation.
		 * 4. Create node hierarchy.
		 * 5. Handle different YAML structures.
		 */
		std::ifstream file(filePath.c_str());
		if (!file.is_open()) {
			close(file);
			throw std::runtime_error("failed to open file");
		}

	}	
}
