#pragma once

namespace kengine {
	class Entity;

    namespace packets {
        struct RemoveEntity {
            Entity & e;
        };
    }
}
