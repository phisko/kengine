#pragma once

#include <functional>
#include "reflection/Reflectible.hpp"

namespace kengine {
    struct InputComponent {
		const std::string type = pmeta_nameof(InputComponent);
        std::function<void(int keycode, bool pressed)> onKey = nullptr;
        std::function<void(float x, float y)> onMouseMove = nullptr;
        std::function<void(int button, float x, float y, bool pressed)> onMouseButton = nullptr;
        std::function<void(float delta, float x, float y)> onMouseWheel = nullptr;

        pmeta_get_class_name(InputComponent);
        pmeta_get_attributes(
            pmeta_reflectible_attribute(&InputComponent::type),
            pmeta_reflectible_attribute(&InputComponent::onKey),
            pmeta_reflectible_attribute(&InputComponent::onMouseMove),
            pmeta_reflectible_attribute(&InputComponent::onMouseButton),
            pmeta_reflectible_attribute(&InputComponent::onMouseWheel)
        );
        pmeta_get_methods();
        pmeta_get_parents();
    };
}