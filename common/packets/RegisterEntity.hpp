#pragma once

namespace kengine {
	class Entity;

    namespace packets {
        struct RegisterEntity {
            Entity & e;
        };
    }
}