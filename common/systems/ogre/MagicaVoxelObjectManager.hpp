#pragma once

#include "Manager.hpp"

namespace Ogre { class SceneManager; }

class MagicaVoxelObjectManager : public Manager {
public:
	MagicaVoxelObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager);

	void registerEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
};