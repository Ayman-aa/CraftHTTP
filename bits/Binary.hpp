#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

class Binary
{
    std::vector< unsigned char > data;
    public:
    Binary();
    Binary( const std::vector< unsigned char > &);
    Binary(const unsigned char *, size_t);

    void append(std::vector< unsigned char >);
    void append(const unsigned char *, size_t);
    size_t find(const std::string &);

    Binary substr(size_t, std::string::size_type = std::string::npos);
    size_t size();
    void erase(size_t, size_t);
    std::string to_string();
};