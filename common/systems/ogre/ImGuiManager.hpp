#pragma once

#include <OgreApplicationContext.h>
#include "Manager.hpp"

class ImGuiManager : public Manager {
public:
	ImGuiManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app);

	virtual bool frameStarted(const Ogre::FrameEvent & e) noexcept;

private:
	kengine::EntityManager & _em;
};