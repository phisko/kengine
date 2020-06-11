#pragma once

#include "Point.hpp"

namespace kengine {
    struct CameraComponent {
        putils::Rect3f frustum{ {}, { 1.f, 1.f, 1.f } };
        float pitch = 0; // Radians
        float yaw = 0; // Radians
		float roll = 0; // Radians

        putils_reflection_class_name(CameraComponent);
        putils_reflection_attributes(
			putils_reflection_attribute(&CameraComponent::frustum),
			putils_reflection_attribute(&CameraComponent::pitch),
			putils_reflection_attribute(&CameraComponent::yaw),
			putils_reflection_attribute(&CameraComponent::roll)
        );
    };
};
