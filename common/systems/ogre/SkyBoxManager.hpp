#pragma once

#include "Manager.hpp"

namespace Ogre { class SceneManager; }

class SkyBoxManager : public Manager {
public:
	SkyBoxManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager);

private:
	void registerEntity(kengine::Entity & e) noexcept override;
	void removeEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
};