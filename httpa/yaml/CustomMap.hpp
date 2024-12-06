/* -- CustomMap.hpp -- */

#pragma once

#include <vector>
#include <cstdlib>
#include "yaml.hpp"

namespace YAML {
	class Node; /* forward declaration of node */
}

class CustomMap{
	private:
		std::vector<std::pair<std::string, YAML::Node*> > m_entries;
	public:
		/* find node by key */
		YAML::Node* find(const std::string& key) {
			for (size_t i = 0; i < m_entries.size(); i++) {
				if (m_entries[i].first == key)
					return m_entries[i].second;
			}
			return NULL;
		}
		/* insert a new entry */
		void insert(const std::string& key, YAML::Node* value) {
			m_entries.push_back(std::make_pair(key, value));
		}
};
