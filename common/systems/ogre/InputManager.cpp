#include "InputManager.hpp"
#include "EntityManager.hpp"

#include "components/InputComponent.hpp"
#include "imgui.h"

InputManager::InputManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app)
	: _em(em), _app(app) {
	app.addInputListener(this);
}

template<typename T>
using input_vector = putils::vector<T, 128>;

static input_vector<OgreBites::KeyboardEvent> g_keyPressedEvents;
static input_vector<OgreBites::KeyboardEvent> g_keyReleasedEvents;
static input_vector<OgreBites::MouseMotionEvent> g_mouseMovedEvents;
static input_vector<OgreBites::MouseButtonEvent> g_mousePressedEvents;
static input_vector<OgreBites::MouseButtonEvent> g_mouseReleasedEvents;
static input_vector<OgreBites::MouseWheelEvent> g_mouseWheelEvents;

void InputManager::execute() noexcept {
	const auto & imgui = ImGui::GetIO();
	for (const auto & [e, input] : _em.getEntities<kengine::InputComponent>()) {
		if (!imgui.WantCaptureKeyboard && input.onKey != nullptr) {
			for (const auto & event : g_keyPressedEvents)
				input.onKey(event.keysym.sym, true);
			for (const auto & event : g_keyReleasedEvents)
				input.onKey(event.keysym.sym, false);
		}

		if (!imgui.WantCaptureMouse) {
			if (input.onMouseMove != nullptr)
				for (const auto & event : g_mouseMovedEvents)
					input.onMouseMove((float)event.x, (float)event.y, (float)event.xrel, (float)event.yrel);

			if (input.onMouseButton != nullptr) {
				for (const auto & event : g_mousePressedEvents)
					input.onMouseButton(event.button, (float)event.x, (float)event.y, true);
				for (const auto & event : g_mouseReleasedEvents)
					input.onMouseButton(event.button, (float)event.x, (float)event.y, false);
			}

			if (input.onMouseWheel != nullptr)
				for (const auto & event : g_mouseWheelEvents)
					input.onMouseWheel((float)event.y, 0.f, 0.f);
		}
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
