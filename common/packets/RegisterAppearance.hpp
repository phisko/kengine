#pragma once

#include <vector>
#include "Point.hpp"
#include "Direction.hpp"

namespace kengine {
    namespace packets {
        struct RegisterAppearance {
            std::string appearance;
            std::string resource;
        };
    }
}
