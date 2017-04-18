#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <comdef.h>
#include "Library.hpp"

class WindowsLibrary final : public putils::Library
{
	// Constructor
public:
	WindowsLibrary(const std::string &name);

	// Destructor
public:
	virtual ~WindowsLibrary() noexcept;

	// Know if library was loaded correctly
public:
	bool				isLoaded() const noexcept override { return (_handle != NULL); }

	// Load a symbol
public:
	void		*loadSymbol(const std::string &name) noexcept override { return (void*)GetProcAddress(_handle, name.c_str()); }

	// Attributes
private:
	HMODULE				_handle;

private:
	WindowsLibrary(const WindowsLibrary &) = delete;
	WindowsLibrary		&operator=(const WindowsLibrary &) = delete;
};

#endif