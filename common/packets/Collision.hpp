#pragma once

namespace kengine {
    namespace packets {
        struct Collision {
            Entity & first;
            Entity & second;
        };
    }
}
