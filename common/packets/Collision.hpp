#pragma once

#include "Entity.hpp"
#include "Point.hpp"

#ifndef KENGINE_QUERY_POSITION_MAX_RESULTS
# define KENGINE_QUERY_POSITION_MAX_RESULTS 64
#endif

namespace kengine {
    namespace packets {
        struct Collision {
            Entity & first;
            Entity & second;
        };

		struct QueryPosition {
			using Results = putils::vector<Entity::ID, KENGINE_QUERY_POSITION_MAX_RESULTS>;

			putils::Point3f pos;
			float radius;
			Results & results;
		};
    }
}
