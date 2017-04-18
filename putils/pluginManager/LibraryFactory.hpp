#pragma once

#include <regex>

#include "WindowsLibrary.hpp"
#include "UnixLibrary.hpp"

namespace putils
{
    namespace LibraryFactory
    {
        putils::Library *make(const std::string &name);
    }
}
