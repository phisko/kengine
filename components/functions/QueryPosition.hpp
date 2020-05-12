#pragma once

#include "BaseFunction.hpp"
#include "Point.hpp"

namespace kengine::functions {
    struct QueryPosition : BaseFunction<
        void (const putils::Point3f & pos, float radius, const EntityIteratorFunc & func)
    > {
        putils_reflection_class_name(QueryPosition);
    };
}