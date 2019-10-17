#pragma once

#include <OgreApplicationContext.h>
#include "Manager.hpp"

class ImGuiManager : public Manager {
public:
	ImGuiManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app);

	void registerEntity(kengine::Entity & e) noexcept override;
	void removeEntity(kengine::Entity & e) noexcept override;
	bool frameStarted(const Ogre::FrameEvent & e) noexcept override;

private:
	kengine::EntityManager & _em;
};