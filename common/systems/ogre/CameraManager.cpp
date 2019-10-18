#include "CameraManager.hpp"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <OgreRenderWindow.h>

#include "components/CameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/AdjustableComponent.hpp"

#include "Utils.hpp"

static float NEAR_PLANE = 0.f;

struct OgreCameraComponent {
	Ogre::SceneNode * node;
	Ogre::Camera * camera;
	Ogre::Viewport * viewPort;
};

static void setTransform(Ogre::SceneNode & node, const kengine::CameraComponent3f & transform) {
	node.setPosition(convert(transform.frustrum.position));
	node.resetOrientation();
	node.yaw(Ogre::Radian(transform.yaw));
	node.pitch(Ogre::Radian(transform.pitch));
	node.roll(Ogre::Radian(transform.roll));
}

CameraManager::CameraManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager, Ogre::RenderWindow & window)
	: _em(em), _sceneManager(sceneManager), _window(window)
{
	onLoad("");
}

void CameraManager::onLoad(const char *) noexcept {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Ogre/Camera] Near plane", &NEAR_PLANE); };
}

void CameraManager::execute() noexcept {
	for (const auto & [e, cam, comp] : _em.getEntities<kengine::CameraComponent3f, OgreCameraComponent>()) {
		setTransform(*comp.node, cam);
		if (NEAR_PLANE < 0.1f)
			NEAR_PLANE = 0.1f;
		comp.camera->setNearClipDistance(NEAR_PLANE);

		// stolen from https://forums.ogre3d.org/viewtopic.php?p=108002#p108002
		const float angle = std::atanf(std::tanf(cam.frustrum.size.y / 2.f) / comp.camera->getAspectRatio()) * 2.f;
		comp.camera->setFOVy(Ogre::Radian(angle));
	}
}

void CameraManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::CameraComponent3f>())
		return;

	const auto & transform = e.get<kengine::TransformComponent3f>();
	const auto & cam = e.get<kengine::CameraComponent3f>();

	OgreCameraComponent comp;
	comp.camera = _sceneManager.createCamera(Ogre::StringConverter::toString(e.id));

	comp.node = _sceneManager.getRootSceneNode()->createChildSceneNode();
	comp.node->attachObject(comp.camera);
	setTransform(*comp.node, cam);

	const auto & viewPortPos = transform.boundingBox.position;
	const auto & viewPortSize = transform.boundingBox.size;
	comp.viewPort = _window.addViewport(comp.camera, (int)viewPortPos.z, viewPortPos.x, viewPortPos.y, viewPortSize.x, viewPortSize.y);

	e += comp;
}

void CameraManager::removeEntity(kengine::Entity & e) noexcept {
	if (!e.has<OgreCameraComponent>())
		return;

	const auto & transform = e.get<kengine::TransformComponent3f>();
	_window.removeViewport((int)transform.boundingBox.position.z);

	const auto & comp = e.get<OgreCameraComponent>();
	_sceneManager.getRootSceneNode()->removeChild(comp.node);
	_sceneManager.destroyCamera(Ogre::StringConverter::toString(e.id));
}
