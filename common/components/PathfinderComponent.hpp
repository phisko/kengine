#pragma once

#include "SerializableComponent.hpp"
#include "Point.hpp"

namespace kengine
{
    class PathfinderComponent : public kengine::SerializableComponent<PathfinderComponent>
    {
    public:
        putils::Point3d dest;
        bool reached = true;
        bool diagonals = true;
        double desiredDistance = 1;
        double maxAvoidance = 0;

        /*
         * Reflectible
         */
    public:
        static const auto get_class_name() { return "PathfinderComponent"; }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&PathfinderComponent::dest),
                    pmeta_reflectible_attribute(&PathfinderComponent::reached),
                    pmeta_reflectible_attribute(&PathfinderComponent::diagonals),
                    pmeta_reflectible_attribute(&PathfinderComponent::desiredDistance),
                    pmeta_reflectible_attribute(&PathfinderComponent::maxAvoidance)
            );
            return table;
        }

        static const auto &get_methods()
        {
            static const auto table = pmeta::make_table();
            return table;
        }

        static const auto &get_parents()
        {
            static const auto table = pmeta::make_table();
            return table;
        }
    };
}
