// Ogre dll warnings
#pragma warning(disable : 4251 4275) 

#include "OgreSystem.hpp"
#include "EntityManager.hpp"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreOverlaySystem.h>

#include "vector.hpp"

#include "Manager.hpp"
#include "InputManager.hpp"
#include "ImGuiManager.hpp"
#include "CameraManager.hpp"
#include "ObjectManager.hpp"

namespace kengine {
	OgreSystem::OgreSystem(EntityManager & em)
		: System(em), _em(em),
		OgreBites::ApplicationContext("Kengine")
	{
	}

	static putils::vector<Manager *, 8> g_managers;

	void OgreSystem::setup() {
		OgreBites::ApplicationContext::setup();
		_sceneManager = getRoot()->createSceneManager();
		Ogre::RTShader::ShaderGenerator::getSingleton().addSceneManager(_sceneManager);
		_sceneManager->addRenderQueueListener(getOverlaySystem());

		g_managers.push_back(new InputManager(_em, *this));
		g_managers.push_back(new ImGuiManager(_em, *this));
		g_managers.push_back(new CameraManager(_em, *_sceneManager, *getRenderWindow()));
		g_managers.push_back(new ObjectManager(_em, *_sceneManager));
	}

	void OgreSystem::execute() noexcept {
		static bool first = true;
		if (first) {
			first = false;

			initApp();

			auto root = getRoot();
			root->getRenderSystem()->_initRenderTargets();
			root->clearEventTimes();
		}

		for (const auto manager : g_managers)
			manager->execute();

		auto root = getRoot();
		if (root->endRenderingQueued() || !root->renderOneFrame()) {
			_em.running = false;
			closeApp();
		}
	}

	bool OgreSystem::frameStarted(const Ogre::FrameEvent & e) {
		const bool ret = OgreBites::ApplicationContext::frameStarted(e);

		for (const auto manager : g_managers)
			if (!manager->frameStarted(e))
				return false;

		return ret;
	}

	void OgreSystem::handle(packets::RegisterEntity p) noexcept {
		for (const auto manager : g_managers)
			manager->registerEntity(p.e);
	}

	void OgreSystem::handle(packets::RemoveEntity p) noexcept {
		for (const auto manager : g_managers)
			manager->removeEntity(p.e);
	}
}