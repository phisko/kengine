// Ogre dll warnings
#pragma warning(disable : 4251 4275) 

#pragma once

namespace kengine {
	class Entity;
	class EntityManager;
}

namespace Ogre {
	struct FrameEvent;
}

class Manager {
public:
	virtual ~Manager() {}

	virtual void registerEntity(kengine::Entity & e) noexcept {}
	virtual void removeEntity(kengine::Entity & e) noexcept {}

	virtual void execute(float time) noexcept {}
	virtual bool frameStarted(const Ogre::FrameEvent & e) noexcept { return true; }
};