#pragma once

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
# define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
#endif

// stl
#include <vector>

// putils
#include "Point.hpp"

// kengine
#include "Entity.hpp"

namespace kengine {
	struct InputBufferComponent {
		struct KeyEvent {
			EntityID window;
			int key;
			bool pressed;
		};
		std::vector<KeyEvent> keys;

		struct ClickEvent {
			EntityID window;
			putils::Point2f pos;
			int button;
			bool pressed;
		};
		std::vector<ClickEvent> clicks;

		struct MouseMoveEvent {
			EntityID window;
			putils::Point2f pos;
			putils::Point2f rel;
		};
		std::vector<MouseMoveEvent> moves;

		struct MouseScrollEvent {
			EntityID window;
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