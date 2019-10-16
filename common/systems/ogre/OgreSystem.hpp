#pragma once

#include "System.hpp"
#include <OgreFrameListener.h>
#include <OgreApplicationContext.h>

namespace Ogre {
	class Root;
	class RenderWindow;
}

namespace kengine {
	class OgreSystem : public System<OgreSystem, packets::RegisterEntity, packets::RemoveEntity>,
		public OgreBites::ApplicationContext, public OgreBites::InputListener
	{
	public:
		OgreSystem(EntityManager & em);
		void execute() noexcept override;
		void setup() override;

		void handle(packets::RegisterEntity p) noexcept;
		void handle(packets::RemoveEntity p) noexcept;

	private:
		bool frameStarted(const Ogre::FrameEvent & e) override;

		void handleInput() noexcept;
		void close() noexcept;

#pragma region Cameras
		void createCamera(Entity & e) noexcept;
		void removeCamera(const Entity & e) noexcept;
#pragma endregion

#pragma region Input
		bool keyPressed(const OgreBites::KeyboardEvent & event) override;
		bool keyReleased(const OgreBites::KeyboardEvent & event) override;
		bool mouseMoved(const OgreBites::MouseMotionEvent & event) override;
		bool mousePressed(const OgreBites::MouseButtonEvent & event) override;
		bool mouseReleased(const OgreBites::MouseButtonEvent & event) override;
		bool mouseWheelRolled(const OgreBites::MouseWheelEvent & event) override;
#pragma endregion

	private:
		EntityManager & _em;

	private:
		Ogre::SceneManager * _sceneManager;
	};
}