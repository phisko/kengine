#pragma once

#include "SerializableComponent.hpp"
#include "Point.hpp"

namespace kengine
{
    class PathfinderComponent : public kengine::SerializableComponent<PathfinderComponent>
    {
    public:
        const std::string type = pmeta_nameof(PathfinderComponent);
        putils::Point3d dest;
        bool reached = true;
        bool diagonals = true;
        double desiredDistance = 1;
        double maxAvoidance = std::numeric_limits<double>::max();

        /*
         * Reflectible
         */
    public:
        static const auto get_class_name() { return pmeta_nameof(PathfinderComponent); }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&PathfinderComponent::type),
                    pmeta_reflectible_attribute(&PathfinderComponent::dest),
                    pmeta_reflectible_attribute(&PathfinderComponent::reached),
                    pmeta_reflectible_attribute(&PathfinderComponent::diagonals),
                    pmeta_reflectible_attribute(&PathfinderComponent::desiredDistance),
                    pmeta_reflectible_attribute(&PathfinderComponent::maxAvoidance)
            );
            return table;
        }
    };
}
