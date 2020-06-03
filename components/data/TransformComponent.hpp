#pragma once

#include "reflection.hpp"
#include "Point.hpp"

namespace kengine {
    struct TransformComponent {
        putils::Rect3f boundingBox{ {}, { 1.f, 1.f, 1.f } };
        float yaw = 0; // Radians
        float pitch = 0; // Radians
		float roll = 0; // Radians

        putils_reflection_class_name(TransformComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&TransformComponent::boundingBox),
                putils_reflection_attribute(&TransformComponent::yaw),
                putils_reflection_attribute(&TransformComponent::pitch),
                putils_reflection_attribute(&TransformComponent::roll)
        );
    };
};
