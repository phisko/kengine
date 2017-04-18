#pragma once

#include <string>
#include <sstream>
#include "fwd.hpp"

namespace putils
{
    template<typename ...Args>
    std::string concat(Args &&...args);

    namespace detail
    {
        template<typename ...Args>
        std::string concat_impl(std::stringstream &s, Args &&...args);

        template<typename T, typename ...Args>
        std::string concat_impl(std::stringstream &s, T &&first, Args &&...args)
        {
            s << FWD(first);
            return concat_impl(s, FWD(args)...);
        };

        template<>
        inline std::string concat_impl(std::stringstream &s)
        {
            return s.str();
        }
    }

    template<typename ...Args>
    std::string concat(Args &&...args)
    {
        std::stringstream s;
        return detail::concat_impl(s, FWD(args)...);
    }
}
