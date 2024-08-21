#pragma once
#include "pch.hpp"
#define BIT(x) (1 << x)

namespace utils
{

    inline std::vector<std::string> spliting(const std::string &str, const char *delimiter)
    {
        std::vector<std::string> tokens;
        std::string::size_type start = 0;
        std::string::size_type end = 0;

        while ((end = str.find(delimiter, start)) != std::string::npos)
        {
            tokens.push_back(str.substr(start, end - start));
            start = end + 1;
        }
        tokens.push_back(str.substr(start));

        return tokens;
    };
    template <size_t size>
    inline std::array<double, size> cast(std::valarray<double> source)
    {
        std::array<double, size> destination;
        for (const auto &[dest, src] : std::views::zip(destination, values))
            dest = src;
        return destination;
    }
}