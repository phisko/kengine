#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
# define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

#include "impl/ID.hpp"
#include "function.hpp"
#include "Point.hpp"

namespace kengine {
    struct InputComponent {
		template<typename T>
		using function = putils::function<T, KENGINE_INPUT_FUNCTION_SIZE>;

        function<void(EntityID window, int keycode, bool pressed)> onKey = nullptr;
        function<void(EntityID window, const putils::Point2f & screenCoordinates, const putils::Point2f & relativeMovement)> onMouseMove = nullptr;
        function<void(EntityID window, int button, const putils::Point2f & screenCoordinates, bool pressed)> onMouseButton = nullptr;
        function<void(EntityID window, float xoffset, float yoffset, const putils::Point2f & screenCoordinates)> onScroll = nullptr;
	};
}

#define refltype kengine::InputComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(onKey),
		putils_reflection_attribute(onMouseMove),
		putils_reflection_attribute(onMouseButton),
		putils_reflection_attribute(onScroll)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::Point2f)
	);
};
#undef refltype