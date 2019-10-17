#include "InputManager.hpp"
#include "EntityManager.hpp"

#include "components/InputComponent.hpp"

InputManager::InputManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app) : _em(em) {
	app.addInputListener(this);
}

static putils::vector<OgreBites::KeyboardEvent, 128> g_keyPressedEvents;
static putils::vector<OgreBites::KeyboardEvent, 128> g_keyReleasedEvents;
static putils::vector<OgreBites::MouseMotionEvent, 128> g_mouseMovedEvents;
static putils::vector<OgreBites::MouseButtonEvent, 128> g_mousePressedEvents;
static putils::vector<OgreBites::MouseButtonEvent, 128> g_mouseReleasedEvents;
static putils::vector<OgreBites::MouseWheelEvent, 128> g_mouseWheelEvents;

void InputManager::execute() noexcept {
	for (const auto & [e, input] : _em.getEntities<kengine::InputComponent>()) {
		if (input.onKey != nullptr) {
			for (const auto & event : g_keyPressedEvents)
				input.onKey(event.keysym.sym, true);
			for (const auto & event : g_keyReleasedEvents)
				input.onKey(event.keysym.sym, false);
		}

		if (input.onMouseMove != nullptr)
			for (const auto & event : g_mouseMovedEvents)
				input.onMouseMove(event.x, event.y);

		if (input.onMouseButton != nullptr) {
			for (const auto & event : g_mousePressedEvents)
				input.onMouseButton(event.button, event.x, event.y, true);
			for (const auto & event : g_mouseReleasedEvents)
				input.onMouseButton(event.button, event.x, event.y, false);
		}

		if (input.onMouseWheel != nullptr)
			for (const auto & event : g_mouseWheelEvents)
				input.onMouseWheel(event.y, 0.f, 0.f);
	}

	g_keyPressedEvents.clear();
	g_keyReleasedEvents.clear();
	g_mouseMovedEvents.clear();
	g_mousePressedEvents.clear();
	g_mouseReleasedEvents.clear();
	g_mouseWheelEvents.clear();
}

bool InputManager::keyPressed(const OgreBites::KeyboardEvent & event) {
	g_keyPressedEvents.push_back(event);
	return true;
}

bool InputManager::keyReleased(const OgreBites::KeyboardEvent & event) {
	g_keyReleasedEvents.push_back(event);
	return true;
}

bool InputManager::mouseMoved(const OgreBites::MouseMotionEvent & event) {
	g_mouseMovedEvents.push_back(event);
	return true;
}

bool InputManager::mousePressed(const OgreBites::MouseButtonEvent & event) {
	g_mousePressedEvents.push_back(event);
	return true;
}

bool InputManager::mouseReleased(const OgreBites::MouseButtonEvent & event) {
	g_mouseReleasedEvents.push_back(event);
	return true;
}

bool InputManager::mouseWheelRolled(const OgreBites::MouseWheelEvent & event) {
	g_mouseWheelEvents.push_back(event);
	return true;
}
