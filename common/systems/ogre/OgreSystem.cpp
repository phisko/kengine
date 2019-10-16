#include "OgreSystem.hpp"
#include "EntityManager.hpp"

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreCamera.h>
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>

#include "ImGuiOverlay.h"
#include "ImGuiInputListener.h"
#include "components/ImGuiComponent.hpp"

#include "components/InputComponent.hpp"

#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"

#include "vector.hpp"
#include "termcolor.hpp"

struct OgreCameraComponent {
	Ogre::SceneNode * node;
	Ogre::Camera * camera;
	Ogre::Viewport * viewPort;
};

static Ogre::Vector3 convert(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
static putils::Point3f convert(const Ogre::Vector3 & p) { return { p.x, p.y, p.z }; }

namespace kengine {
	OgreSystem::OgreSystem(EntityManager & em)
		: System(em), _em(em),
		OgreBites::ApplicationContext("Kengine")
	{
	}

	void OgreSystem::setup() {
		OgreBites::ApplicationContext::setup();

		addInputListener(this);

		_sceneManager = getRoot()->createSceneManager();

		auto imguiOverlay = new Ogre::kengine::ImGuiOverlay;
		imguiOverlay->setZOrder(300);
		imguiOverlay->show();
		Ogre::OverlayManager::getSingleton().addOverlay(imguiOverlay);

		_sceneManager->addRenderQueueListener(getOverlaySystem());

		Ogre::RTShader::ShaderGenerator::getSingleton().addSceneManager(_sceneManager);

		addInputListener(new OgreBites::kengine::ImGuiInputListener);
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

		handleInput();

		auto root = getRoot();
		if (root->endRenderingQueued() || !root->renderOneFrame())
			close();
	}

	bool OgreSystem::frameStarted(const Ogre::FrameEvent & e) {
		Ogre::kengine::ImGuiOverlay::NewFrame(e);

		for (const auto & [e, comp] : _em.getEntities<ImGuiComponent>())
			comp.display(GImGui);

		return OgreBites::ApplicationContext::frameStarted(e);
	}

	void OgreSystem::close() noexcept {
		_em.running = false;
		closeApp();
	}

	void OgreSystem::handle(packets::RegisterEntity p) noexcept {
		if (p.e.has<CameraComponent3f>())
			createCamera(p.e);
	}

	void OgreSystem::handle(packets::RemoveEntity p) noexcept {
		if (p.e.has<OgreCameraComponent>())
			removeCamera(p.e);
	}

#pragma region Cameras
	void OgreSystem::createCamera(Entity & e) noexcept {
		const auto & transform = e.get<TransformComponent3f>();
		const auto & cam = e.get<CameraComponent3f>();

		OgreCameraComponent comp;
		comp.camera = _sceneManager->createCamera(Ogre::StringConverter::toString(e.id));

		comp.node = _sceneManager->getRootSceneNode()->createChildSceneNode();
		comp.node->attachObject(comp.camera);
		comp.node->setPosition(convert(cam.frustrum.position));
		comp.node->yaw(Ogre::Radian(cam.yaw));
		comp.node->pitch(Ogre::Radian(cam.pitch));
		comp.node->roll(Ogre::Radian(cam.roll));

		const auto & viewPortPos = transform.boundingBox.position;
		const auto & viewPortSize = transform.boundingBox.size;
		comp.viewPort = getRenderWindow()->addViewport(comp.camera, viewPortPos.z, viewPortPos.x, viewPortPos.y, viewPortSize.x, viewPortSize.y);

		e += comp;
	}

	void OgreSystem::removeCamera(const Entity & e) noexcept {
		const auto & transform = e.get<TransformComponent3f>();
		getRenderWindow()->removeViewport(transform.boundingBox.position.z);

		const auto & comp = e.get<OgreCameraComponent>();
		_sceneManager->getRootSceneNode()->removeChild(comp.node);
		_sceneManager->destroyCamera(Ogre::StringConverter::toString(e.id));
	}
#pragma endregion

#pragma region Input
	static putils::vector<OgreBites::KeyboardEvent, 128> g_keyPressedEvents;
	static putils::vector<OgreBites::KeyboardEvent, 128> g_keyReleasedEvents;
	static putils::vector<OgreBites::MouseMotionEvent, 128> g_mouseMovedEvents;
	static putils::vector<OgreBites::MouseButtonEvent, 128> g_mousePressedEvents;
	static putils::vector<OgreBites::MouseButtonEvent, 128> g_mouseReleasedEvents;
	static putils::vector<OgreBites::MouseWheelEvent, 128> g_mouseWheelEvents;

	void OgreSystem::handleInput() noexcept {
		for (const auto & [e, input] : _em.getEntities<InputComponent>()) {
			if (input.onKey != nullptr) {
				for (const auto & event : g_keyPressedEvents)
					input.onKey(event.keysym.sym, true);
				for (const auto & event : g_keyReleasedEvents)
					input.onKey(event.keysym.sym, false);
			}

			if (input.onMouseMove != nullptr)
				for (const auto & event : g_mouseMovedEvents)
					input.onMouseMove(event.x, event.y);

			if (input.onMouseButton != nullptr) {
				for (const auto & event : g_mousePressedEvents)
					input.onMouseButton(event.button, event.x, event.y, true);
				for (const auto & event : g_mouseReleasedEvents)
					input.onMouseButton(event.button, event.x, event.y, false);
			}

			if (input.onMouseWheel != nullptr)
				for (const auto & event : g_mouseWheelEvents)
					input.onMouseWheel(event.y, 0.f, 0.f);
		}

		g_keyPressedEvents.clear();
		g_keyReleasedEvents.clear();
		g_mouseMovedEvents.clear();
		g_mousePressedEvents.clear();
		g_mouseReleasedEvents.clear();
		g_mouseWheelEvents.clear();
	}

	bool OgreSystem::keyPressed(const OgreBites::KeyboardEvent & event) {
		g_keyPressedEvents.push_back(event);
		return true;
	}

	bool OgreSystem::keyReleased(const OgreBites::KeyboardEvent & event) {
		g_keyReleasedEvents.push_back(event);
		return true;
	}

	bool OgreSystem::mouseMoved(const OgreBites::MouseMotionEvent & event) {
		g_mouseMovedEvents.push_back(event);
		return true;
	}

	bool OgreSystem::mousePressed(const OgreBites::MouseButtonEvent & event) {
		g_mousePressedEvents.push_back(event);
		return true;
	}

	bool OgreSystem::mouseReleased(const OgreBites::MouseButtonEvent & event) {
		g_mouseReleasedEvents.push_back(event);
		return true;
	}

	bool OgreSystem::mouseWheelRolled(const OgreBites::MouseWheelEvent & event) {
		g_mouseWheelEvents.push_back(event);
		return true;
	}
#pragma endregion
}