#pragma once

#include "reflection.hpp"
#include "Point.hpp"

namespace kengine {
    class TransformComponent {
    public:
        TransformComponent(const putils::Point3f & pos = { 0, 0, 0 },
                           const putils::Point3f & size = { 1, 1, 1 })
                : boundingBox(pos, size)
		{}

        TransformComponent(const putils::Rect3f & rect)
                : boundingBox(rect) {}

        putils::Rect3f boundingBox;
        float pitch = 0; // Radians
        float yaw = 0; // Radians
		float roll = 0; // Radians

        /*
         * Reflectible
         */

    public:
        putils_reflection_class_name(TransformComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&TransformComponent::boundingBox),
                putils_reflection_attribute(&TransformComponent::pitch),
                putils_reflection_attribute(&TransformComponent::yaw),
                putils_reflection_attribute(&TransformComponent::roll)
        );
    };
};
