#pragma once

#include <sstream>

namespace putils
{
    std::string read_stream(auto &&istream)
    {
        std::ostringstream ret;

        while (istream && istream.peek() >= 0)
            ret << (char)istream.get();

        return ret.str();
    }
}