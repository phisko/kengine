#pragma once

#ifndef KENGINE_LOG_MSG_SIZE
# define KENGINE_LOG_MSG_SIZE 128
#endif

#include "string.hpp"
#include "reflection.hpp"

namespace kengine {
    namespace packets {
        struct Log {
            static constexpr char stringName[] = "LogPacketString";
            putils::string<KENGINE_LOG_MSG_SIZE, stringName> msg;

            Log(std::string_view msg = "") : msg(msg) {}

            /*
             * Reflectible
             */

            putils_reflection_class_name(Log);
            putils_reflection_attributes(
                    putils_reflection_attribute(&Log::msg)
            );
        };
    }
}
