#pragma once

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
# define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
#endif

// stl
#include <vector>

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "Point.hpp"

namespace kengine {
	struct InputBufferComponent {
		struct KeyEvent {
			entt::entity window;
			int key;
			bool pressed;
		};
		std::vector<KeyEvent> keys;

		struct ClickEvent {
			entt::entity window;
			putils::Point2f pos;
			int button;
			bool pressed;
		};
		std::vector<ClickEvent> clicks;

		struct MouseMoveEvent {
			entt::entity window;
			putils::Point2f pos;
			putils::Point2f rel;
		};
		std::vector<MouseMoveEvent> moves;

		struct MouseScrollEvent {
			entt::entity window;
			float xoffset;
			float yoffset;
			putils::Point2f pos;
		};
		std::vector<MouseScrollEvent> scrolls;
	};
}

#define refltype kengine::InputBufferComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype