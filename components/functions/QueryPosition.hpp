#pragma once

#include "BaseFunction.hpp"
#include "Entity.hpp"
#include "Point.hpp"

#ifndef KENGINE_QUERY_POSITION_MAX_RESULTS
# define KENGINE_QUERY_POSITION_MAX_RESULTS 64
#endif

namespace kengine::functions {
    struct QueryPosition : BaseFunction<
        putils::vector<Entity::ID, KENGINE_QUERY_POSITION_MAX_RESULTS>(const putils::Point3f & pos, float radius)
    > {
        putils_reflection_class_name(OnCollision);
    };
}