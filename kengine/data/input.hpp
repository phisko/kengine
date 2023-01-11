#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
#define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/function.hpp"
#include "putils/point.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [putils::point2f]
	struct input {
		template<typename T>
		using function = putils::function<T, KENGINE_INPUT_FUNCTION_SIZE>;

		function<void(entt::handle window, int keycode, bool pressed)> on_key = nullptr;
		function<void(entt::handle window, const putils::point2f & screen_coordinates, const putils::point2f & relative_movement)> on_mouse_move = nullptr;
		function<void(entt::handle window, int button, const putils::point2f & screen_coordinates, bool pressed)> on_mouse_button = nullptr;
		function<void(entt::handle window, float xoffset, float yoffset, const putils::point2f & screen_coordinates)> on_scroll = nullptr;
	};
}

#include "input.reflection.hpp"