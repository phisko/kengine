#pragma once

#include <string>

namespace kengine
{
    namespace packets
    {
        struct Log : putils::Reflectible<Log>
        {
            std::string msg;

            /*
             * Reflectible
             */

            static const auto get_class_name() { return "Log"; }
            static const auto &get_attributes()
            {
                static const auto table = pmeta::make_table("msg", &Log::msg);
                return table;
            }

            static const auto &get_methods()
            {
                static const auto table = pmeta::make_table();
                return table;
            }

            static const auto &get_parents()
            {
                static const auto table = pmeta::make_table();
                return table;
            }
        };
    }
}
