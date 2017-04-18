#pragma once

#include <sstream>
#include "read_stream.hpp"

namespace putils
{
    std::string to_string(auto &&obj)
    {
        std::stringstream s;
        s << FWD(obj);
        return s.str();
    }

    inline std::string to_string(std::istream &s)
    {
        return read_stream(s);
    }
}