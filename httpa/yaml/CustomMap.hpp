/* -- CustomMap.hpp -- */

#pragma once

#include <vector>
#include <cstdlib>
#include "yaml.hpp"

class Node{};

class CustomMap{
	private:
		std::vector<std::pair<std::string, Node*> > m_entries;
	public:
		/* find node by key */
		Node* find(const std::string& key) {
			for (size_t i = 0; i < m_entries.size(); i++) {
				if (m_entries[i].first == key)
					return m_entries[i].second;
			}
			return NULL;
		}
		/* insert a new entry */
		void insert(const std::string& key, Node* value) {
			m_entries.push_back(std::make_pair(key, value));
		}
};
