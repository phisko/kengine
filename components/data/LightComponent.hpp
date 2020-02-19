#pragma once

#include "reflection.hpp"
#include "Point.hpp"
#include "Color.hpp"
#include "vector.hpp"

#ifndef KENGINE_MAX_CSM_COUNT
# define KENGINE_MAX_CSM_COUNT 5
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

		putils_reflection_attributes(
			putils_reflection_attribute(&LightComponent::color),
			putils_reflection_attribute(&LightComponent::diffuseStrength),
			putils_reflection_attribute(&LightComponent::specularStrength),
			putils_reflection_attribute(&LightComponent::castShadows),
			putils_reflection_attribute(&LightComponent::shadowPCFSamples),
			putils_reflection_attribute(&LightComponent::shadowMapSize),
			putils_reflection_attribute(&LightComponent::shadowMapBias)
		);
	};

	struct DirLightComponent : LightComponent {
		static constexpr char vectorName[] = "DirLightComponentCascadeVector";
		using vector = putils::vector<float, KENGINE_MAX_CSM_COUNT, vectorName>;

		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float ambientStrength = .1f;
		vector cascadeEnds = { 50.f };
		float shadowCasterMaxDistance = 100.f;

		putils_reflection_class_name(DirLightComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&DirLightComponent::direction),
			putils_reflection_attribute(&DirLightComponent::ambientStrength),
			putils_reflection_attribute(&DirLightComponent::cascadeEnds),
			putils_reflection_attribute(&DirLightComponent::shadowCasterMaxDistance)
		);
		putils_reflection_parents(
			putils_reflection_type(LightComponent)
		);
		putils_reflection_used_types(
			putils_reflection_type(vector)
		);
	};

	struct PointLightComponent : LightComponent {
		float range = 1000.f;
		float constant = 1.f;
		float linear = .09f;
		float quadratic = .032f;

		putils_reflection_class_name(PointLightComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&PointLightComponent::range),
			putils_reflection_attribute(&PointLightComponent::constant),
			putils_reflection_attribute(&PointLightComponent::linear),
			putils_reflection_attribute(&PointLightComponent::quadratic)
		);
		putils_reflection_parents(
			putils_reflection_type(LightComponent)
		);
	};

	struct SpotLightComponent : PointLightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float cutOff = 1.f;
		float outerCutOff = 1.2f;

		putils_reflection_class_name(SpotLightComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&SpotLightComponent::direction),
			putils_reflection_attribute(&SpotLightComponent::cutOff),
			putils_reflection_attribute(&SpotLightComponent::outerCutOff)
		);
		putils_reflection_parents(
			putils_reflection_type(PointLightComponent)
		);
	};
}
