#pragma once

#include "Manager.hpp"

namespace Ogre {
	class SceneManager;
	class RenderWindow;
};

class CameraManager : public Manager {
public:
	CameraManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager, Ogre::RenderWindow & window);

	void registerEntity(kengine::Entity & e) noexcept;
	void removeEntity(kengine::Entity & e) noexcept;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
	Ogre::RenderWindow & _window;
};