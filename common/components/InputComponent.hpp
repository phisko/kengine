#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
# define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

#include "reflection/Reflectible.hpp"
#include "function.hpp"

namespace kengine {
    struct InputComponent : kengine::not_serializable {
        putils::function<void(int keycode, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> onKey = nullptr;
        putils::function<void(float x, float y), KENGINE_INPUT_FUNCTION_SIZE> onMouseMove = nullptr;
        putils::function<void(int button, float x, float y, bool pressed), KENGINE_INPUT_FUNCTION_SIZE> onMouseButton = nullptr;
        putils::function<void(float delta, float x, float y), KENGINE_INPUT_FUNCTION_SIZE> onMouseWheel = nullptr;

        pmeta_get_class_name(InputComponent);
        pmeta_get_attributes(
            pmeta_reflectible_attribute(&InputComponent::onKey),
            pmeta_reflectible_attribute(&InputComponent::onMouseMove),
            pmeta_reflectible_attribute(&InputComponent::onMouseButton),
            pmeta_reflectible_attribute(&InputComponent::onMouseWheel)
        );
        pmeta_get_methods();
        pmeta_get_parents();
    };
}