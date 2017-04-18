#pragma once

#ifdef _WIN32

#include <direct.h>
#include <windows.h>
#include <datetimeapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include "ADirectory.hpp"

namespace putils
{
    class WindowsDirectory final : public putils::ADirectory
    {
        // Constructor
    public:
        WindowsDirectory(const std::string &path) noexcept
            :
        _path(path),
        _goOn(true)
    {
        if (_path[_path.length() - 1] != '/')
            _path = _path + "/";
        _handle = FindFirstFile((_path + "*").c_str(), &_ffd);
    }

        // Destructor
    public:
        virtual ~WindowsDirectory() noexcept
           {
        if (_handle != nullptr)
            FindClose(_handle);
    }

        // Read next directory entry
    public:
        std::unique_ptr<File>   getNextFile() noexcept override
            {
        if (_goOn == false || _handle == INVALID_HANDLE_VALUE)
            return nullptr;

        if (FindNextFile(_handle, &_ffd) == false)
            _goOn = false;

        std::string			file(_ffd.cFileName);
        std::string			fullPath(_path + file);

        if (file == "." || file == "..")
            return getNextFile();

        bool				isDir(
            (bool)((_ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        );

        return std::unique_ptr<File>(new File({	file, fullPath, isDir }));
    }

        // Attributes
    private:
        std::string		_path;
        HANDLE			_handle;
        WIN32_FIND_DATA	_ffd;
        bool			_goOn;

        // Coplien
    public:
        WindowsDirectory(const WindowsDirectory &) = delete;
        WindowsDirectory	&operator=(const WindowsDirectory &) = delete;
    };
}


#endif