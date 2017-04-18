#pragma once

#include <string>

namespace putils
{
    inline std::string chop(const std::string &str)
    {
        auto end = str.find_last_not_of(" \t\n\r");

        if (end == std::string::npos)
            return str;
        else
            return str.substr(0, str.find_last_not_of(" \t\n\r") + 1);
    }
}
