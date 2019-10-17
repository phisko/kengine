#include "ImGuiManager.hpp"
#include "EntityManager.hpp"

#include <OgreOverlayManager.h>

#include "ImGuiOverlay.h"
#include "ImGuiInputListener.h"
#include "components/ImGuiComponent.hpp"

ImGuiManager::ImGuiManager(kengine::EntityManager & em, OgreBites::ApplicationContext & app) : _em(em) {
	auto imguiOverlay = new Ogre::kengine::ImGuiOverlay;
	imguiOverlay->setZOrder(300);
	imguiOverlay->show();
	Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay);
	app.addInputListener(new OgreBites::kengine::ImGuiInputListener);
}

bool ImGuiManager::frameStarted(const Ogre::FrameEvent & e) noexcept {
	Ogre::kengine::ImGuiOverlay::NewFrame(e);

	for (const auto & [e, comp] : _em.getEntities<kengine::ImGuiComponent>())
		comp.display(GImGui);

	return true;
}