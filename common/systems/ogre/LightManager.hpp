#pragma once

#include "Manager.hpp"

namespace Ogre {
	class SceneManager;
};

class LightManager : public Manager {
public:
	LightManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager);

	void onLoad(const char *) noexcept override;

	void execute(float time) noexcept override;
	void registerEntity(kengine::Entity & e) noexcept override;
	void removeEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
	Ogre::SceneManager & _sceneManager;
};