#include "Binary.hpp"
Binary::Binary() {}
Binary::Binary(const std::vector<unsigned char>& initialData) : data(initialData) {}
Binary::Binary(const unsigned char* rawData, size_t size) : data(rawData, rawData + size) {}

void Binary::append(std::vector<unsigned char> newData)
{
    data.insert(data.end(), newData.begin(), newData.end());
}
void Binary::append(const unsigned char* rawData, size_t size)
{
    data.insert(data.end(), rawData, rawData + size);
}
size_t Binary::find(const std::string &to_find)
{
    if(to_find.empty())
        return std::string::npos;
    std::vector<unsigned char>::iterator it = std::search(data.begin(), data.end(), to_find.begin(), to_find.end());
    if(it == data.end())
        return std::string::npos;
    return std::distance(data.begin(), it);
}
Binary Binary::substr(size_t start, std::string::size_type length)
{
    length = std::min(length,data.size() - start);
    if( start > data.size())
        return Binary();
    return Binary(&data[start], length);
}
void Binary::erase(size_t start, size_t length)
{
    if(start >= data.size())
        return;
    data.erase(data.begin() + start, data.begin() + std::min(data.size(), start + length));
}
std::string Binary::to_string()
{
    return std::string(data.begin(), data.end());
}
size_t Binary::size()
{
    return data.size();
}