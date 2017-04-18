#pragma once

#include "Point.hpp"

namespace putils
{
    using Direction = Point<int>;

    namespace Directions
    {
        static const Point<int> Up = { 0, 1 };
        static const Point<int> Down = { 0, -1 };
        static const Point<int> Left = { -1, 0 };
        static const Point<int> Right = { 1, 0 };
        static const Point<int> NoDirection = { 0, 0 };
    }
}
