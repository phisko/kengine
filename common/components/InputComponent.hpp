#pragma once

#include <functional>
#include "SerializableComponent.hpp"

namespace kengine {
    class InputComponent : public SerializableComponent<InputComponent> {
    public:
        InputComponent() = default;

    public:
        std::function<void(int keycode, bool pressed)> onKey = nullptr;
        std::function<void(int x, int y)> onMouseMove = nullptr;
        std::function<void(int button, int x, int y, bool pressed)> onMouseButton = nullptr;

    public:
        pmeta_get_class_name(InputComponent);
        pmeta_get_attributes(
            pmeta_reflectible_attribute(&InputComponent::onKey),
            pmeta_reflectible_attribute(&InputComponent::onMouseMove),
            pmeta_reflectible_attribute(&InputComponent::onMouseButton)
        );
        pmeta_get_methods();
        pmeta_get_parents();
    };
}