#pragma once

#include "Manager.hpp"

namespace Ogre {
	class SceneManager;
};

class AssimpObjectManager : public Manager {
public:
	AssimpObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager);

	void registerEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
};
