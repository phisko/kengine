
#pragma once

#include "Manager.hpp"

namespace Ogre {
	class SceneManager;
};

class ObjectManager : public Manager {
public:
	ObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager);

	void execute(float time) noexcept override;
	void registerEntity(kengine::Entity & e) noexcept override;
	void removeEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
};
