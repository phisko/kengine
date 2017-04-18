#pragma once

#include "WindowsDirectory.hpp"
#include "UnixDirectory.hpp"

namespace putils
{
#ifdef _WIN32
    using Directory = WindowsDirectory;
#elif defined(__unix__)
    using Directory = UnixDirectory;
#endif
}
