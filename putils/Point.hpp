#pragma once

#include <cmath>
#include "hash.hpp"

namespace putils
{
    template<typename Precision, std::size_t Dimensions = 2>
    struct Point
    {
        Precision x;
        Precision y;

        template<typename P>
        bool operator==(const Point<P> &rhs) const noexcept { return x == rhs.x && y == rhs.y; }

        template<typename P>
        bool operator!=(const Point<P> &rhs) const noexcept { return !(*this == rhs); }

        template<typename P>
        Point operator+(const Point<P> &rhs) const noexcept { return { x + rhs.x, y + rhs.y }; }

        template<typename P>
        Point &operator+=(const Point<P> &rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }

        template<typename P>
        Point operator-(const Point<P> &rhs) const noexcept { return { x - rhs.x, y - rhs.y }; }

        template<typename P>
        Point &operator-=(const Point<P> &rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }

        template<typename P>
        Precision distanceTo(const Point<P> &rhs) const noexcept
        {
            return std::sqrt(
                    std::pow(x - rhs.x, 2) +
                    std::pow(y - rhs.y, 2)
            );
        }
    };

    template<typename Precision, std::size_t Dimensions = 2>
    struct Rect
    {
        Point<Precision> topLeft;
        Point<Precision> size;

        template<typename P>
        bool operator==(const Rect<P> &rhs) { return topLeft == rhs.topLeft && size == rhs.size; }

        template<typename P>
        bool operator!=(const Rect<P> &rhs) { return !(*this == rhs); }
    };

    template<typename Precision>
    bool intersect(const Rect<Precision, 2> &first, const Rect<Precision, 2> &second)
    {
        return !(first.topLeft.x >= second.topLeft.x + second.size.x ||
                first.topLeft.x + first.size.x <= second.topLeft.x ||
                first.topLeft.y >= second.topLeft.y + second.size.y ||
                first.topLeft.y + first.size.y <= second.topLeft.y
        );
    }

    template<typename Precision>
    struct Point<Precision, 3>
    {
        Precision x;
        Precision y;
        Precision z;

        template<typename P>
        bool operator==(const Point<P> &rhs) const noexcept
        { return x == rhs.x && y == rhs.y && z == rhs.z; }

        template<typename P>
        bool operator!=(const Point<P> &rhs) const noexcept { return !(*this == rhs); }

        template<typename P>
        Point operator+(const Point<P> &rhs) const noexcept { return { x + rhs.x, y + rhs.y, z + rhs.z }; }

        template<typename P>
        Point &operator+=(const Point<P> &rhs) noexcept { x += rhs.x; y += rhs.x; z += rhs.z; return *this; }

        template<typename P>
        Point operator-(const Point<P> &rhs) const noexcept { return { x - rhs.x, y - rhs.y, z - rhs.z }; }

        template<typename P>
        Point &operator-=(const Point<P> &rhs) noexcept { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        template<typename P>
        Precision distanceTo(const Point<P> &rhs) const noexcept
        {
            return std::sqrt(
                    std::pow(x - rhs.x, 2) +
                    std::pow(y - rhs.y, 2) +
                    std::pow(z - rhs.z, 2)
            );
        }
    };

    template<typename Precision>
    struct Rect<Precision, 3>
    {
        Point<Precision, 3> topLeft;
        Point<Precision, 3> size;

        template<typename P>
        bool operator==(const Rect<P> &rhs) { return topLeft == rhs.topLeft && size == rhs.size; }

        template<typename P>
        bool operator!=(const Rect<P> &rhs) { return !(*this == rhs); }
    };

    template<typename Precision>
    bool intersect(const Rect<Precision, 3> &first, const Rect<Precision, 3> &second)
    {
        return !(first.topLeft.x >= second.topLeft.x + second.size.x ||
                first.topLeft.x + first.size.x <= second.topLeft.x ||
                first.topLeft.y >= second.topLeft.y + second.size.y ||
                first.topLeft.y + first.size.y <= second.topLeft.y ||
                first.topLeft.z >= second.topLeft.z + second.size.z ||
                first.topLeft.z + first.size.z <= second.topLeft.z
        );
    }
}

namespace std
{
    template<typename Precision>
    struct hash<putils::Point<Precision>>
    {
        size_t operator()(const putils::Point<Precision> &coord) const noexcept
        {
            return putils::PairHash().operator()(std::make_pair(coord.x, coord.y));
        }
    };
}

