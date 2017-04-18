#pragma once

#if defined(__unix__) || defined(__APPLE__)

#include <dlfcn.h>
#include "Library.hpp"

namespace putils
{
    class UnixLibrary final : public Library
    {
        // Constructor
    public:
        UnixLibrary(const std::string &name);

        // Destructor
    public:
        virtual ~UnixLibrary() noexcept;

        // Know if library was loaded correctly
    public:
        bool isLoaded() const noexcept override { return (_handle != nullptr); }

        // Load a symbol
    public:
        void *loadSymbol(const std::string &name) noexcept override { return dlsym(_handle, name.c_str()); }

        // Attributes
    private:
        void *_handle;

        // Coplien
    public:
        UnixLibrary(const UnixLibrary &) = delete;

        UnixLibrary &operator=(const UnixLibrary &) = delete;
    };
}

#endif