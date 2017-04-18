#ifdef _WIN32

#include <algorithm>
#include <stdio.h>
#include <sstream>
#include "WindowsLibrary.hpp"
#include "putils/Utils.hpp"

WindowsLibrary::WindowsLibrary(const std::string &name)
	:
	Library(name),
	_handle(LoadLibrary(name.c_str()))
{
   /* std::string copy(name);

    std::replace(copy.begin(), copy.end(), '/', '\\');
    _handle = LoadLibrary(copy.c_str());
	*/
	if (_handle == nullptr)
	{
	    std::stringstream s;
		s << "Failed to load library '" << name << "': " << putils::GetLastErrorAsString();
		throw std::runtime_error(s.str());
	}
}

WindowsLibrary::~WindowsLibrary() noexcept
{
	if (_handle != nullptr)
		FreeLibrary(_handle);
}

#endif
