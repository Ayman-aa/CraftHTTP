/* -- Content.hpp -- */

#ifndef CONTENT_HPP
#define CONTENT_HPP

#include <vector>
#include <string>

class Content {
private:
    std::vector<unsigned char> data;

public:
    Content();
    Content(const std::vector<unsigned char>& initData);
    
    void append(const std::vector<unsigned char>& newData);
    std::string toStr() const;
    void erase(size_t start, size_t length);
    size_t size();
};

#endif
