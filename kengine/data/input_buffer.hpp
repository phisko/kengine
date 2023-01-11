#pragma once

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
#define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
#endif

// stl
#include <vector>

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/point.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::key_event, refltype::click_event, refltype::mouse_move_event, refltype::mouse_scroll_event]
	struct input_buffer {
		//! putils reflect all
		struct key_event {
			entt::entity window;
			int key;
			bool pressed;
		};
		std::vector<key_event> keys;

		//! putils reflect all
		//! used_types: [putils::point2f]
		struct click_event {
			entt::entity window;
			putils::point2f pos;
			int button;
			bool pressed;
		};
		std::vector<click_event> clicks;

		//! putils reflect all
		//! used_types: [putils::point2f]
		struct mouse_move_event {
			entt::entity window;
			putils::point2f pos;
			putils::point2f rel;
		};
		std::vector<mouse_move_event> moves;

		//! putils reflect all
		//! used_types: [putils::point2f]
		struct mouse_scroll_event {
			entt::entity window;
			float xoffset;
			float yoffset;
			putils::point2f pos;
		};
		std::vector<mouse_scroll_event> scrolls;
	};
}

#include "input_buffer.reflection.hpp"