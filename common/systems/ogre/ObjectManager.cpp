#include "ObjectManager.hpp"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include "components/TransformComponent.hpp"
#include "components/GraphicsComponent.hpp"

#include "Utils.hpp"

struct OgreObjectComponent {
	Ogre::SceneNode * node;
	Ogre::Entity * entity;
};

static void setTransform(Ogre::SceneNode & node, const kengine::TransformComponent3f & transform) {
	node.setPosition(convert(transform.boundingBox.position));
	node.yaw(Ogre::Radian(transform.yaw));
	node.pitch(Ogre::Radian(transform.pitch));
	node.roll(Ogre::Radian(transform.roll));
}

ObjectManager::ObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{
	_sceneManager.setAmbientLight(Ogre::ColourValue(.5f, .5f, .5f));
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("resources/Sinbad.zip", "Zip");
}

void ObjectManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::GraphicsComponent>())
		return;

	const auto & graphics = e.get<kengine::GraphicsComponent>();

	OgreObjectComponent comp;
	comp.entity = _sceneManager.createEntity(graphics.appearance.c_str());
	comp.node = _sceneManager.getRootSceneNode()->createChildSceneNode();
	comp.node->attachObject(comp.entity);
	setTransform(*comp.node, e.get<kengine::TransformComponent3f>());

	e += comp;
}

void ObjectManager::removeEntity(kengine::Entity & e) noexcept {
	if (!e.has<OgreObjectComponent>())
		return;

	const auto & comp = e.get<OgreObjectComponent>();
	_sceneManager.getRootSceneNode()->removeChild(comp.node);
	_sceneManager.destroyEntity(comp.entity);
}
