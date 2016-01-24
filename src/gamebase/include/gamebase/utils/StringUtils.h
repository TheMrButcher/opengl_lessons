#pragma once

#include <boost/lexical_cast.hpp>
#include <string>

namespace gamebase {

template <typename T>
std::string toString(const T& t)
{
    return boost::lexical_cast<std::string>(t);
}

inline float toFloat(const std::string& str)
{
    return boost::lexical_cast<float>(str);
}

inline int toInt(const std::string& str)
{
    return boost::lexical_cast<int>(str);
}

inline double toDouble(const std::string& str)
{
    return boost::lexical_cast<double>(str);
}

inline std::string addSlash(const std::string& path)
{
    if (path.empty() || path.back() == '\\')
        return path;
    return path + '\\';
}

}
