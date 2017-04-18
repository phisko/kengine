#pragma once

#include <unistd.h>

namespace putils
{
    // Go to the directory containing file in currentPath
    inline void goToBinDir(const std::string &currentPath) noexcept
    {
        try
        {
            size_t last = currentPath.find_last_of("/\\");
            chdir(currentPath.substr(0, last).c_str());
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}