#pragma once

// Ogre dll warnings
#pragma warning(disable : 4251) 
#pragma warning(disable : 4275)

#include "System.hpp"
#include <OgreFrameListener.h>
#include <OgreApplicationContext.h>

namespace Ogre {
	class Root;
	class RenderWindow;
}

namespace kengine {
	class OgreSystem : public System<OgreSystem, packets::RegisterEntity, packets::RemoveEntity>,
		public OgreBites::ApplicationContext
	{
	public:
		OgreSystem(EntityManager & em);
		void execute() noexcept override;
		void setup() override;

		void handle(packets::RegisterEntity p) noexcept;
		void handle(packets::RemoveEntity p) noexcept;

	private:
		bool frameStarted(const Ogre::FrameEvent & e) override;

	private:
		EntityManager & _em;

	private:
		Ogre::SceneManager * _sceneManager;
	};
}