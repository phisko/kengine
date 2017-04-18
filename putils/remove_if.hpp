#pragma once

namespace putils
{
    // Remove items fitting a condition
    template<typename Container, typename Predicate>
    void remove_if(Container &container, Predicate &&pred)
    {
        auto it = container.begin();
        while (it != container.end())
        {
            if (pred(*it))
                it = container.erase(it);
            else
                ++it;
        }
    }
}
