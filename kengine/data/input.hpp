#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
# define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/function.hpp"
#include "putils/point.hpp"

namespace kengine::data {
    struct input {
		template<typename T>
		// using function = putils::function<T, KENGINE_INPUT_FUNCTION_SIZE>;
		using function = std::function<T>;

        function<void(entt::handle window, int keycode, bool pressed)> on_key = nullptr;
        function<void(entt::handle window, const putils::point2f & screen_coordinates, const putils::point2f & relative_movement)> on_mouse_move = nullptr;
        function<void(entt::handle window, int button, const putils::point2f & screen_coordinates, bool pressed)> on_mouse_button = nullptr;
        function<void(entt::handle window, float xoffset, float yoffset, const putils::point2f & screen_coordinates)> on_scroll = nullptr;
	};
}

#define refltype kengine::data::input
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(on_key),
		putils_reflection_attribute(on_mouse_move),
		putils_reflection_attribute(on_mouse_button),
		putils_reflection_attribute(on_scroll)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::point2f)
	);
};
#undef refltype