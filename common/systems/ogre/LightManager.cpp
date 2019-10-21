#include "LightManager.hpp"
#include "EntityManager.hpp"

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include "components/LightComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/AdjustableComponent.hpp"

#include "Utils.hpp"

static Ogre::ShadowTechnique SHADOW_TYPE = Ogre::ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE;

struct OgreLightComponent {
	Ogre::SceneNode * node = nullptr;
	Ogre::Light * light;
};

LightManager::LightManager(kengine::EntityManager & em, Ogre::SceneManager & sceneManager)
	: _em(em), _sceneManager(sceneManager)
{
	onLoad("");
}

void LightManager::onLoad(const char *) noexcept {
	_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Ogre] Shadow type", &SHADOW_TYPE); };
}

void LightManager::execute() noexcept {
	_sceneManager.setShadowTechnique(SHADOW_TYPE);

	Ogre::ColourValue ambientColor{ 0.f, 0.f, 0.f, 0.f };
	for (const auto & [e, light, comp] : _em.getEntities<kengine::DirLightComponent, OgreLightComponent>()) {
		const auto color = convert(light.color);
		ambientColor += color * light.ambientStrength;

		comp.light->setDiffuseColour(color * light.diffuseStrength);
		comp.light->setSpecularColour(color * light.specularStrength);

		comp.node->setDirection(convert(light.direction), Ogre::SceneNode::TS_WORLD);
	}
	_sceneManager.setAmbientLight(ambientColor);

	for (const auto & [e, light, transform, comp] : _em.getEntities<kengine::PointLightComponent, kengine::TransformComponent3f, OgreLightComponent>()) {
		const auto color = convert(light.color);

		comp.light->setDiffuseColour(color * light.diffuseStrength);
		comp.light->setSpecularColour(color * light.specularStrength);

		comp.light->setAttenuation(light.range, light.constant, light.linear, light.quadratic);
		comp.node->setPosition(convert(transform.boundingBox.position));
	}

	for (const auto & [e, light, transform, comp] : _em.getEntities<kengine::SpotLightComponent, kengine::TransformComponent3f, OgreLightComponent>()) {
		const auto color = convert(light.color);

		comp.light->setDiffuseColour(color * light.diffuseStrength);
		comp.light->setSpecularColour(color * light.specularStrength);

		comp.light->setAttenuation(light.range, light.constant, light.linear, light.quadratic);
		comp.node->setPosition(convert(transform.boundingBox.position));

		comp.node->setDirection(convert(light.direction), Ogre::SceneNode::TS_WORLD);
		comp.light->setSpotlightRange(Ogre::Radian(light.cutOff), Ogre::Radian(light.outerCutOff));
	}
}

void LightManager::registerEntity(kengine::Entity & e) noexcept {
	const bool isDirLight = e.has<kengine::DirLightComponent>();
	const bool isPointLight = e.has<kengine::PointLightComponent>();
	const bool isSpotLight = e.has<kengine::SpotLightComponent>();

	if (!isDirLight && !isPointLight && !isSpotLight)
		return;

	OgreLightComponent comp;
	comp.light = _sceneManager.createLight();
	comp.node = _sceneManager.getRootSceneNode()->createChildSceneNode();
	comp.node->attachObject(comp.light);
	comp.node->setPosition({ 0.f, 0.f, 0.f });

	if (isDirLight)
		comp.light->setType(Ogre::Light::LT_DIRECTIONAL);
	else if (isPointLight)
		comp.light->setType(Ogre::Light::LT_POINT);
	else if (isSpotLight)
		comp.light->setType(Ogre::Light::LT_SPOTLIGHT);

	e += comp;
}

void LightManager::removeEntity(kengine::Entity & e) noexcept {
	if (e.has<OgreLightComponent>()) {
		const auto & comp = e.get<OgreLightComponent>();
		_sceneManager.destroyLight(comp.light);
		_sceneManager.getRootSceneNode()->removeChild(comp.node);
	}
}
