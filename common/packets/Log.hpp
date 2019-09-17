#pragma once

#include <string>
#include "putils/reflection/Reflectible.hpp"

namespace kengine {
    namespace packets {
        struct Log {
            std::string msg;

            Log(std::string_view msg = "") : msg(msg) {}

            /*
             * Reflectible
             */

            pmeta_get_class_name(Log);
            pmeta_get_attributes(
                    pmeta_reflectible_attribute(&Log::msg)
            );
            pmeta_get_methods();
            pmeta_get_parents();
        };
    }
}
