#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
# define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

#include "not_serializable.hpp"
#include "reflection/Reflectible.hpp"
#include "function.hpp"

namespace kengine {
    struct InputComponent : kengine::not_serializable {
		template<typename T>
		using function = putils::function<T, KENGINE_INPUT_FUNCTION_SIZE>;

        function<void(int keycode, bool pressed)> onKey = nullptr;
        function<void(float x, float y, float xrel, float yrel)> onMouseMove = nullptr;
        function<void(int button, float x, float y, bool pressed)> onMouseButton = nullptr;
        function<void(float delta, float x, float y)> onMouseWheel = nullptr;

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