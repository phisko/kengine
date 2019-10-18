#pragma once

#include "Manager.hpp"

namespace Ogre {
	class SceneManager;
	class RenderWindow;
};

class CameraManager : public Manager {
public:
	CameraManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager, Ogre::RenderWindow & window);

	void onLoad(const char *) noexcept override;
	void execute() noexcept override;
	void registerEntity(kengine::Entity & e) noexcept override;
	void removeEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
	Ogre::RenderWindow & _window;
};