#pragma once

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
# define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
#endif

// stl
#include <vector>

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::data {
	struct input_buffer {
		struct key_event {
			entt::entity window;
			int key;
			bool pressed;
		};
		std::vector<key_event> keys;

		struct click_event {
			entt::entity window;
			putils::point2f pos;
			int button;
			bool pressed;
		};
		std::vector<click_event> clicks;

		struct mouse_move_event {
			entt::entity window;
			putils::point2f pos;
			putils::point2f rel;
		};
		std::vector<mouse_move_event> moves;

		struct mouse_scroll_event {
			entt::entity window;
			float xoffset;
			float yoffset;
			putils::point2f pos;
		};
		std::vector<mouse_scroll_event> scrolls;
	};
}

#define refltype kengine::data::input_buffer
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype