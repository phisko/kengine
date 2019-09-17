#pragma once

#include "reflection/Reflectible.hpp"
#include "Point.hpp"
#include "Color.hpp"

namespace kengine {
	struct LightComponent {
		putils::NormalizedColor color;
		float diffuseStrength = 1.f;
		float specularStrength = .1f;
	};

	struct DirLightComponent : LightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float ambientStrength = .1f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DirLightComponent::color),
			pmeta_reflectible_attribute(&DirLightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&DirLightComponent::specularStrength),
			pmeta_reflectible_attribute(&DirLightComponent::direction),
			pmeta_reflectible_attribute(&DirLightComponent::ambientStrength)
		);
		pmeta_get_class_name(DirLightComponent);
		pmeta_get_methods();
		pmeta_get_parents();
	};

	struct PointLightComponent : LightComponent {
		float constant = 1.f;
		float linear = .09f;
		float quadratic = .032f;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PointLightComponent::color),
			pmeta_reflectible_attribute(&PointLightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&PointLightComponent::specularStrength),
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
		float cutOff = std::cos(12.5f);
		float outerCutOff = std::cos(15.f);

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&SpotLightComponent::color),
			pmeta_reflectible_attribute(&SpotLightComponent::diffuseStrength),
			pmeta_reflectible_attribute(&SpotLightComponent::specularStrength),
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
