#pragma once

#include <string>
#include "putils/reflection/Reflectible.hpp"

namespace kengine
{
    namespace packets
    {
        struct Log
        {
            std::string msg;

            Log(std::string_view msg) : msg(msg) {}

            /*
             * Reflectible
             */

            static const auto get_class_name() { return pmeta_nameof(Log); }
            static const auto &get_attributes()
            {
                static const auto table = pmeta::make_table(
                        pmeta_reflectible_attribute(&Log::msg)
                );
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
