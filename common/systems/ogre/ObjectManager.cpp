#include "ObjectManager.hpp"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>

#include "OgreObjectComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/ModelComponent.hpp"

#include "Utils.hpp"
#include "file_extension.hpp"

static void setTransform(Ogre::SceneNode & node, const kengine::TransformComponent3f & transform, const kengine::ModelComponent * model) {
	node.resetOrientation();

	auto position = convert(transform.boundingBox.position);
	auto scale = convert(transform.boundingBox.size);
	auto yaw = Ogre::Radian(transform.yaw);
	auto pitch = Ogre::Radian(transform.pitch);
	auto roll = Ogre::Radian(transform.roll);

	if (model != nullptr) {
		position += convert(model->boundingBox.position);
		scale *= convert(model->boundingBox.size);
		yaw += Ogre::Radian(model->yaw);
		pitch += Ogre::Radian(model->pitch);
		roll += Ogre::Radian(model->roll);
	}

	node.setPosition(position);
	node.setScale(scale);
	node.yaw(yaw);
	node.pitch(pitch);
	node.roll(roll);
}

ObjectManager::ObjectManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{
	_sceneManager.setAmbientLight(Ogre::ColourValue(.5f, .5f, .5f));
}

void ObjectManager::execute(float time) noexcept {
	for (const auto & [e, transform, graphics, comp] : _em.getEntities<kengine::TransformComponent3f, kengine::GraphicsComponent, OgreObjectComponent>()) {
		const kengine::ModelComponent * model = nullptr;
		if (graphics.model != kengine::Entity::INVALID_ID)
			model = &_em.getEntity(graphics.model).get<kengine::ModelComponent>();

		setTransform(*comp.node, transform, model);
	}
}

void ObjectManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::GraphicsComponent>())
		return;

	auto & graphics = e.get<kengine::GraphicsComponent>();
	if (graphics.model == kengine::Entity::INVALID_ID)
		for (const auto & [e, model] : _em.getEntities<kengine::ModelComponent>())
			if (model.file == graphics.appearance)
				graphics.model = e.id;

	if (putils::file_extension(graphics.appearance) != "mesh")
		return;

	OgreObjectComponent comp;

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
