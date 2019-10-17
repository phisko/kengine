
#pragma once

#include "Manager.hpp"

namespace Ogre {
	class SceneManager;
};

class ObjectManager : public Manager {
public:
	ObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager);

	void registerEntity(kengine::Entity & e) noexcept;
	void removeEntity(kengine::Entity & e) noexcept;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
};
