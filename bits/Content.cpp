#include "Content.hpp"
Content::Content() {}
Content::Content(const std::vector<unsigned char>& initialData) : data(initialData) {}
Content::Content(const unsigned char* rawData, size_t size) : data(rawData, rawData + size) {}

void Content::append(std::vector<unsigned char> newData)
{
    data.insert(data.end(), newData.begin(), newData.end());
}
void Content::append(const unsigned char* rawData, size_t size)
{
    data.insert(data.end(), rawData, rawData + size);
}
size_t Content::find(const std::string &to_find)
{
    if(to_find.empty())
        return std::string::npos;
    std::vector<unsigned char>::iterator it = std::search(data.begin(), data.end(), to_find.begin(), to_find.end());
    if(it == data.end())
        return std::string::npos;
    return std::distance(data.begin(), it);
}
Content Content::substr(size_t start, std::string::size_type length)
{
    length = std::min(length,data.size() - start);
    if( start > data.size())
        return Content();
    return Content(&data[start], length);
}
void Content::erase(size_t start, size_t length)
{
    if(start >= data.size())
        return;
    data.erase(data.begin() + start, data.begin() + std::min(data.size(), start + length));
}
std::string Content::to_string()
{
    return std::string(data.begin(), data.end());
}
size_t Content::size()
{
    return data.size();
}