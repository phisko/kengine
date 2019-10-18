// Ogre dll warnings
#pragma warning(disable : 4251 4275) 

#include "ObjectManager.hpp"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>

#include "OgreObjectComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/GraphicsComponent.hpp"

#include "Utils.hpp"
#include "file_extension.hpp"

static void setTransform(Ogre::SceneNode & node, const kengine::TransformComponent3f & transform) {
	node.setPosition(convert(transform.boundingBox.position));
	node.setScale(convert(transform.boundingBox.size));

	node.resetOrientation();
	node.yaw(Ogre::Radian(transform.yaw));
	node.pitch(Ogre::Radian(transform.pitch));
	node.roll(Ogre::Radian(transform.roll));
}

ObjectManager::ObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{
	_sceneManager.setAmbientLight(Ogre::ColourValue(.5f, .5f, .5f));
}

void ObjectManager::execute() noexcept {
	for (const auto & [e, transform, comp] : _em.getEntities<kengine::TransformComponent3f, OgreObjectComponent>())
		setTransform(*comp.node, transform);
}

void ObjectManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::GraphicsComponent>())
		return;

	const auto & graphics = e.get<kengine::GraphicsComponent>();

	OgreObjectComponent comp;

	if (strcmp(putils::file_extension(graphics.appearance), "mesh") != 0)
		return;

	comp.entity = _sceneManager.createEntity(graphics.appearance.c_str());
	comp.node = _sceneManager.getRootSceneNode()->createChildSceneNode();
	comp.node->attachObject(comp.entity);

	e += comp;
}

void ObjectManager::removeEntity(kengine::Entity & e) noexcept {
	if (!e.has<OgreObjectComponent>())
		return;

	const auto & comp = e.get<OgreObjectComponent>();
	_sceneManager.getRootSceneNode()->removeChild(comp.node);
	_sceneManager.destroyEntity(comp.entity);
}
