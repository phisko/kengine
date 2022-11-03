#pragma once

// reflection
#include "reflection.hpp"

// putils
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
        float shadowMapMaxBias = .1f;
        float shadowMapMinBias = .01f;
	};

	struct DirLightComponent : LightComponent {
		static constexpr char vectorName[] = "DirLightComponentCascadeVector";
		using vector = putils::vector<float, KENGINE_MAX_CSM_COUNT, vectorName>;

		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float ambientStrength = .1f;
        float lightSphereDistance = 500.f;

		vector cascadeEnds = { 50.f };
		float shadowCasterMaxDistance = 100.f;
	};

	struct PointLightComponent : LightComponent {
		float range = 1000.f;
		float attenuationConstant = 1.f;
		float attenuationLinear = .09f;
		float attenuationQuadratic = .032f;
	};

	struct SpotLightComponent : PointLightComponent {
		putils::Vector3f direction = { 0.f, -1.f, 0.f };
		float cutOff = 1.f;
		float outerCutOff = 1.2f;
	};
}

#define refltype kengine::LightComponent
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute(color),
		putils_reflection_attribute(diffuseStrength),
		putils_reflection_attribute(specularStrength),
		putils_reflection_attribute(castShadows),
		putils_reflection_attribute(shadowPCFSamples),
		putils_reflection_attribute(shadowMapSize),
		putils_reflection_attribute(shadowMapMaxBias),
        putils_reflection_attribute(shadowMapMinBias)
	);
};
#undef refltype

#define refltype kengine::DirLightComponent 
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(direction),
		putils_reflection_attribute(ambientStrength),
        putils_reflection_attribute(lightSphereDistance),
		putils_reflection_attribute(cascadeEnds),
		putils_reflection_attribute(shadowCasterMaxDistance)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::LightComponent)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::vector)
	);
};
#undef refltype

#define refltype kengine::PointLightComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(range),
		putils_reflection_attribute(attenuationConstant),
		putils_reflection_attribute(attenuationLinear),
		putils_reflection_attribute(attenuationQuadratic)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::LightComponent)
	);
};
#undef refltype

#define refltype kengine::SpotLightComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(direction),
		putils_reflection_attribute(cutOff),
		putils_reflection_attribute(outerCutOff)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::PointLightComponent)
	);
};
#undef refltype
