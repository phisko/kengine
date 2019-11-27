#include "InputSystem.hpp"
#include "components/InputComponent.hpp"

#ifndef KENGINE_INPUT_MAX_BUFFERED_EVENTS
# define KENGINE_INPUT_MAX_BUFFERED_EVENTS 128
#endif

namespace kengine {
	template<typename T>
	using EventVector = putils::vector<T, KENGINE_INPUT_MAX_BUFFERED_EVENTS>;

	static EventVector<InputSystem::KeyEvent> g_keys;
	static EventVector<InputSystem::ClickEvent> g_clicks;
	static EventVector<InputSystem::MouseMoveEvent> g_moves;
	static EventVector<InputSystem::MouseScrollEvent> g_scrolls;

	InputSystem::InputSystem(EntityManager & em)
		: System(em), _em(em)
	{}

	void InputSystem::execute() noexcept {
		for (const auto &[e, comp] : _em.getEntities<InputComponent>()) {
			for (const auto & e : g_keys)
				if (comp.onKey != nullptr)
					comp.onKey(e.window, e.key, e.pressed);

			if (comp.onMouseButton != nullptr)
				for (const auto & e : g_clicks)
					comp.onMouseButton(e.window, e.button, e.pos, e.pressed);

			if (comp.onMouseMove != nullptr)
				for (const auto & e : g_moves)
					comp.onMouseMove(e.window, e.pos, e.rel);

			if (comp.onScroll != nullptr)
				for (const auto & e : g_scrolls)
					comp.onScroll(e.window, e.xoffset, e.yoffset, e.pos);
		}
		g_keys.clear();
		g_clicks.clear();
		g_moves.clear();
		g_scrolls.clear();
	}

	void InputSystem::addEvent(const KeyEvent & e) {
		if (!g_keys.full())
			g_keys.push_back(e);
	}

	void InputSystem::addEvent(const ClickEvent & e) {
		if (!g_clicks.full())
			g_clicks.push_back(e);
	}

	void InputSystem::addEvent(const MouseMoveEvent & e) {
		if (!g_moves.full())
			g_moves.push_back(e);
	}

	void InputSystem::addEvent(const MouseScrollEvent & e) {
		if (!g_scrolls.full())
			g_scrolls.push_back(e);
	}
}