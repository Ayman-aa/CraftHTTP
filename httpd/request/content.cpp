/* -- content.cpp -- */

#include "../includes/content.hpp"

Content::Content() {};

Content::Content(const std::vector<unsigned char>& initData): data(initData) {}

void Content::append(const std::vector<unsigned char>& newData) {
	data.insert(data.end(), newData.begin(), newData.end());
}

std::string Content::toStr() const {
	return std::string(data.begin(), data.end());
}

void Content::erase(size_t start, size_t length) {
	if (start < data.size()) {
		data.erase(data.begin() + start,
				data.begin() + std::min(start + length, data.size()));
	}
}

size_t Content::size() { return data.size(); }
