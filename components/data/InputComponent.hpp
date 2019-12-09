#pragma once

#ifndef KENGINE_INPUT_FUNCTION_SIZE
# define KENGINE_INPUT_FUNCTION_SIZE 64
#endif

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
# define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
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

	struct InputBufferComponent {
		template<typename T>
		using EventVector = putils::vector<T, KENGINE_INPUT_MAX_BUFFERED_EVENTS>;

		struct KeyEvent {
			Entity::ID window;
			int key;
			bool pressed;
		};
		EventVector<KeyEvent> keys;

		struct ClickEvent {
			Entity::ID window;
			putils::Point2f pos;
			int button;
			bool pressed;
		};
		EventVector<ClickEvent> clicks;

		struct MouseMoveEvent {
			Entity::ID window;
			putils::Point2f pos;
			putils::Point2f rel;
		};
		EventVector<MouseMoveEvent> moves;

		struct MouseScrollEvent {
			Entity::ID window;
			float xoffset;
			float yoffset;
			putils::Point2f pos;
		};
		EventVector<MouseScrollEvent> scrolls;
	};
}