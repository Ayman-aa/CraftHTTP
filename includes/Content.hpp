#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

class Content {
	private:
		std::vector<unsigned char> data;
	public:
		Content();
		Content(const std::vector<unsigned char>& initialData);
		Content(const unsigned char* rawData, size_t size);

		void append(const std::vector<unsigned char>& newData);
		void append(const unsigned char* newData, size_t size);

		size_t find(const std::string& pattern) const;

		Content substr(size_t start, std::string::size_type length = std::string::npos) const;

		size_t size();
		void erase(size_t start, size_t length);
		std::string toStr() const;
		// Overloaded operators
		Content& operator=(const Content& other);
		friend std::ostream& operator<<(std::ostream& os, const Content& Content);
};

#endif 