#pragma once

#include "System.hpp"
#include "Point.hpp"

namespace kengine {
	class InputSystem : public System<InputSystem> {
	public:
		InputSystem(EntityManager & em);
		void execute() noexcept override;

		struct KeyEvent {
			Entity::ID window;
			int key;
			bool pressed;
		};
		void addEvent(const KeyEvent & e);

		struct ClickEvent {
			Entity::ID window;
			putils::Point2f pos;
			int button;
			bool pressed;
		};
		void addEvent(const ClickEvent & e);

		struct MouseMoveEvent {
			Entity::ID window;
			putils::Point2f pos;
			putils::Point2f rel;
		};
		void addEvent(const MouseMoveEvent & e);

		struct MouseScrollEvent {
			Entity::ID window;
			float xoffset;
			float yoffset;
			putils::Point2f pos;
		};
		void addEvent(const MouseScrollEvent & e);

	private:
		EntityManager & _em;
	};
}