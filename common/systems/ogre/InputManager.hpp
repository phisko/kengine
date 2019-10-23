#pragma once

#include "Manager.hpp"
#include <OgreApplicationContext.h>

class InputManager : public Manager, public OgreBites::InputListener {
public:
	InputManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app);

	void execute() noexcept override;

	bool keyPressed(const OgreBites::KeyboardEvent & event) override;
	bool keyReleased(const OgreBites::KeyboardEvent & event) override;
	bool mouseMoved(const OgreBites::MouseMotionEvent & event) override;
	bool mousePressed(const OgreBites::MouseButtonEvent & event) override;
	bool mouseReleased(const OgreBites::MouseButtonEvent & event) override;
	bool mouseWheelRolled(const OgreBites::MouseWheelEvent & event) override;

private:
	kengine::EntityManager & _em;
	OgreBites::ApplicationContext & _app;
};