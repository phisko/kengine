#include "ImGuiManager.hpp"
#include "EntityManager.hpp"

#include <OgreOverlayManager.h>

#include "ImGuiOverlay.h"
#include "ImGuiInputListener.h"
#include "components/ImGuiComponent.hpp"
#include "components/CameraComponent.hpp"

ImGuiManager::ImGuiManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app) : _em(em) {
	auto imguiOverlay = new Ogre::kengine::ImGuiOverlay;
	imguiOverlay->setZOrder(300);
	imguiOverlay->show();
	Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay);
	app.addInputListener(new OgreBites::kengine::ImGuiInputListener);
}

static size_t g_nbCameras = 0;

bool ImGuiManager::frameStarted(const Ogre::FrameEvent & e) noexcept {
	if (g_nbCameras == 0)
		return true;

	Ogre::kengine::ImGuiOverlay::NewFrame(e);

	for (const auto & [e, comp] : _em.getEntities<kengine::ImGuiComponent>())
		comp.display(GImGui);

	return true;
}

void ImGuiManager::registerEntity(kengine::Entity & e) noexcept {
	if (e.has<kengine::CameraComponent3f>())
		++g_nbCameras;
}

void ImGuiManager::removeEntity(kengine::Entity & e) noexcept {
	if (e.has<kengine::CameraComponent3f>())
		--g_nbCameras;
}
