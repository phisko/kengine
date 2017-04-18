#include "UnixLibrary.hpp"

#if defined(__unix__)

#include <sstream>

namespace putils
{
    UnixLibrary::UnixLibrary(const std::string &name)
            :
            Library(name),
            _handle(dlopen(name.c_str(), RTLD_NOW))
    {
        if (_handle == nullptr)
        {
            std::stringstream s;
            s << "Failed to load library '" << name << "': " << dlerror();
            throw std::runtime_error(s.str());
        }
    }

    UnixLibrary::~UnixLibrary() noexcept
    {
        if (_handle != nullptr)
            dlclose(_handle);
    }
}

#endif
