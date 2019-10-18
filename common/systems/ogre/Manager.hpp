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

	virtual void onLoad(const char * path) noexcept {}
	virtual void onSave(const char * path) noexcept {}

	virtual void registerEntity(kengine::Entity & e) noexcept {}
	virtual void removeEntity(kengine::Entity & e) noexcept {}

	virtual void execute() noexcept {}
	virtual bool frameStarted(const Ogre::FrameEvent & e) noexcept { return true; }
};