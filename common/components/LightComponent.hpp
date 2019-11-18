#pragma once

#include "reflection.hpp"
#include "Point.hpp"
#include "Color.hpp"

#ifndef KENGINE_CSM_COUNT
# define KENGINE_CSM_COUNT 3
#endif

namespace kengine {
	struct LightComponent {
		putils::NormalizedColor color;
		float diffuseStrength = 1.f;
		float specularStrength = .1f;
		bool castShadows = true;
		int shadowPCFSamples = 1;
		int shadowMapSize = 1024;
		float shadowMapBias = .00001f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&LightComponent::color),
			pmeta_reflectible_attribute(&LightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&LightComponent::specularStrength),
			pmeta_reflectible_attribute(&LightComponent::castShadows),
			pmeta_reflectible_attribute(&LightComponent::shadowPCFSamples),
			pmeta_reflectible_attribute(&LightComponent::shadowMapSize),
			pmeta_reflectible_attribute(&LightComponent::shadowMapBias)
		);
	};

	struct DirLightComponent : LightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float ambientStrength = .1f;
		float cascadeEnds[KENGINE_CSM_COUNT] = { 50.f };
		float shadowCasterMaxDistance = 100.f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DirLightComponent::shadowCasterMaxDistance),
			pmeta_reflectible_attribute(&DirLightComponent::direction),
			pmeta_reflectible_attribute(&DirLightComponent::ambientStrength),
			pmeta_reflectible_attribute(&DirLightComponent::cascadeEnds)
		);
		pmeta_get_class_name(DirLightComponent);
		pmeta_get_methods();
		pmeta_get_parents(
			pmeta_reflectible_parent(LightComponent)
		);
	};

	struct PointLightComponent : LightComponent {
		float range = 1000.f;
		float constant = 1.f;
		float linear = .09f;
		float quadratic = .032f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PointLightComponent::range),
			pmeta_reflectible_attribute(&PointLightComponent::constant),
			pmeta_reflectible_attribute(&PointLightComponent::linear),
			pmeta_reflectible_attribute(&PointLightComponent::quadratic)
		);
		pmeta_get_class_name(PointLightComponent);
		pmeta_get_methods();
		pmeta_get_parents(
			pmeta_reflectible_parent(LightComponent)
		);
	};

	struct SpotLightComponent : PointLightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float cutOff = 1.f;
		float outerCutOff = 1.2f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SpotLightComponent::direction),
			pmeta_reflectible_attribute(&SpotLightComponent::cutOff),
			pmeta_reflectible_attribute(&SpotLightComponent::outerCutOff)
		);
		pmeta_get_class_name(SpotLightComponent);
		pmeta_get_methods();
		pmeta_get_parents(
			pmeta_reflectible_parent(PointLightComponent)
		);
	};
}
