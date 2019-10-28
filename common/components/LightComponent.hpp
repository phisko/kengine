#pragma once

#include "reflection/Reflectible.hpp"
#include "Point.hpp"
#include "Color.hpp"

namespace kengine {
	struct LightComponent {
		putils::NormalizedColor color;
		float diffuseStrength = 1.f;
		float specularStrength = .1f;
		bool castShadows = true;
		int shadowPCFSamples = 1;
		int shadowMapSize = 1024;
	};

	struct DirLightComponent : LightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float ambientStrength = .1f;
		float shadowDistance = 50.f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DirLightComponent::color),
			pmeta_reflectible_attribute(&DirLightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&DirLightComponent::specularStrength),
			pmeta_reflectible_attribute(&DirLightComponent::castShadows),
			pmeta_reflectible_attribute(&DirLightComponent::shadowPCFSamples),
			pmeta_reflectible_attribute(&DirLightComponent::shadowMapSize),

			pmeta_reflectible_attribute(&DirLightComponent::direction),
			pmeta_reflectible_attribute(&DirLightComponent::ambientStrength),
			pmeta_reflectible_attribute(&DirLightComponent::shadowDistance)
		);
		pmeta_get_class_name(DirLightComponent);
		pmeta_get_methods();
		pmeta_get_parents();
	};

	struct PointLightComponent : LightComponent {
		float range = 1000.f;
		float constant = 1.f;
		float linear = .09f;
		float quadratic = .032f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PointLightComponent::color),
			pmeta_reflectible_attribute(&PointLightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&PointLightComponent::specularStrength),
			pmeta_reflectible_attribute(&PointLightComponent::castShadows),
			pmeta_reflectible_attribute(&PointLightComponent::shadowPCFSamples),
			pmeta_reflectible_attribute(&PointLightComponent::shadowMapSize),

			pmeta_reflectible_attribute(&PointLightComponent::range),
			pmeta_reflectible_attribute(&PointLightComponent::constant),
			pmeta_reflectible_attribute(&PointLightComponent::linear),
			pmeta_reflectible_attribute(&PointLightComponent::quadratic)
		);
		pmeta_get_class_name(PointLightComponent);
		pmeta_get_methods();
		pmeta_get_parents();
	};

	struct SpotLightComponent : PointLightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float cutOff = 1.f;
		float outerCutOff = 1.2f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SpotLightComponent::color),
			pmeta_reflectible_attribute(&SpotLightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&SpotLightComponent::specularStrength),
			pmeta_reflectible_attribute(&SpotLightComponent::castShadows),
			pmeta_reflectible_attribute(&SpotLightComponent::shadowPCFSamples),
			pmeta_reflectible_attribute(&SpotLightComponent::shadowMapSize),

			pmeta_reflectible_attribute(&SpotLightComponent::range),
			pmeta_reflectible_attribute(&SpotLightComponent::constant),
			pmeta_reflectible_attribute(&SpotLightComponent::linear),
			pmeta_reflectible_attribute(&SpotLightComponent::quadratic),

			pmeta_reflectible_attribute(&SpotLightComponent::direction),
			pmeta_reflectible_attribute(&SpotLightComponent::cutOff),
			pmeta_reflectible_attribute(&SpotLightComponent::outerCutOff)
		);
		pmeta_get_class_name(SpotLightComponent);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}
