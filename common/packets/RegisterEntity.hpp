#pragma once

#include "Entity.hpp"

namespace kengine {
    namespace packets {
        struct RegisterEntity {
            Entity & e;
        };
    }
}