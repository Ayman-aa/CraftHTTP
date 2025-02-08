#include "../includes/Content.hpp"

Content::Content() {}
Content::Content(const std::vector<unsigned char>& initialData) : data(initialData) {}
Content::Content(const unsigned char* rawData, size_t size) : data(rawData, rawData + size) {}

void Content::append(const std::vector<unsigned char>& newData)
{
	data.insert(data.end(), newData.begin(), newData.end());
}

void Content::append(const unsigned char* newData, size_t size)
{
	data.insert(data.end(), newData, newData + size);
}

size_t Content::find(const std::string& pattern) const
{
	if (pattern.empty()) 
		return std::string::npos; 

	std::vector<unsigned char>::const_iterator result = std::search(data.begin(), data.end(), pattern.begin(), pattern.end());

	if (result != data.end()) 
		return std::distance(data.begin(), result);
	else 
		return std::string::npos;  
}

Content Content::substr(size_t start, std::string::size_type length) const
{
	length = std::min(length, data.size() - start);
	if (start > data.size())
		return Content();
	return Content(&data[start], length);
}

std::ostream& operator<<(std::ostream& os, const Content& Content)
{
	for (std::vector<unsigned char>::const_iterator it = Content.data.begin(); it != Content.data.end(); ++it) {
		os << std::hex << static_cast<int>(*it) << " ";
	}
	os << std::dec;
	return os;
}

Content& Content::operator=(const Content& other)
{
	if (this != &other) 
		this->data = other.data;

	return *this;
}

std::string Content::toStr() const 
{
	std::string result(data.begin(), data.end());
	return result;
}

void Content::erase(size_t start, size_t length)
{
	if (start < data.size()) 
		data.erase(data.begin() + start, data.begin() + std::min(start + length, data.size()));
}

size_t Content::size() {return data.size();}
