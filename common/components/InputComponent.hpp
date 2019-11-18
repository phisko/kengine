#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
# define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

#include "reflection.hpp"
#include "function.hpp"
#include "Point.hpp"

namespace kengine {
    struct InputComponent {
		template<typename T>
		using function = putils::function<T, KENGINE_INPUT_FUNCTION_SIZE>;

        function<void(Entity::ID window, int keycode, bool pressed)> onKey = nullptr;
        function<void(Entity::ID window, const putils::Point2f & screenCoordinates, const putils::Point2f & relativeMovement)> onMouseMove = nullptr;
        function<void(Entity::ID window, int button, const putils::Point2f & screenCoordinates, bool pressed)> onMouseButton = nullptr;
        function<void(Entity::ID window, float xoffset, float yoffset, const putils::Point2f & screenCoordinates)> onScroll = nullptr;

        putils_reflection_class_name(InputComponent);
        putils_reflection_attributes(
            putils_reflection_attribute(&InputComponent::onKey),
            putils_reflection_attribute(&InputComponent::onMouseMove),
            putils_reflection_attribute(&InputComponent::onMouseButton),
            putils_reflection_attribute(&InputComponent::onScroll)
        );
    };
}