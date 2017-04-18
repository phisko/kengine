#pragma once

#include <algorithm>

namespace putils
{
    template<typename Container>
    void erase(Container &container, auto &&val)
    {
        container.erase(std::find(container.begin(), container.end(), FWD(val)));
    }
}