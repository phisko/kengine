#pragma once

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
# define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
#endif

#include "vector.hpp"
#include "Point.hpp"
#include "Entity.hpp"

namespace kengine {
	struct InputBufferComponent {
		template<typename T>
		using EventVector = putils::vector<T, KENGINE_INPUT_MAX_BUFFERED_EVENTS>;

		struct KeyEvent {
			EntityID window;
			int key;
			bool pressed;
		};
		EventVector<KeyEvent> keys;

		struct ClickEvent {
			EntityID window;
			putils::Point2f pos;
			int button;
			bool pressed;
		};
		EventVector<ClickEvent> clicks;

		struct MouseMoveEvent {
			EntityID window;
			putils::Point2f pos;
			putils::Point2f rel;
		};
		EventVector<MouseMoveEvent> moves;

		struct MouseScrollEvent {
			EntityID window;
			float xoffset;
			float yoffset;
			putils::Point2f pos;
		};
		EventVector<MouseScrollEvent> scrolls;
	};
}

#define refltype kengine::InputBufferComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype