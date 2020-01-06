#pragma once

#include "Point.hpp"

namespace kengine {
    class CameraComponent {
    public:
        CameraComponent(const putils::Point3f & pos = { 0, 0, 0 },
                        const putils::Point3f & size = { 1, 1, 1 })
                : frustum(pos, size)
		{}

        CameraComponent(const putils::Rect3f & rect)
                : frustum(rect)
		{}

        putils::Rect3f frustum;
        float pitch = 0; // Radians
        float yaw = 0; // Radians
		float roll = 0; // Radians

        /*
         * Reflectible
         */

    public:
        putils_reflection_class_name(CameraComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&CameraComponent::frustum),
                putils_reflection_attribute(&CameraComponent::pitch),
                putils_reflection_attribute(&CameraComponent::yaw),
                putils_reflection_attribute(&CameraComponent::roll)
        );
    };
};
