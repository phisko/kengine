#pragma once

#include <memory>

#include "putils/opengl/Program.hpp"
#include "putils/opengl/RAII.hpp"
#include "data/LightComponent.hpp"

namespace kengine {
	class Entity;

	struct ShaderComponent {
		std::unique_ptr<putils::gl::Program> shader;
		bool enabled = true;
	};

	struct GBufferShaderComponent : ShaderComponent {};
	struct LightingShaderComponent : ShaderComponent {};
	struct PostLightingShaderComponent : ShaderComponent {};
	struct PostProcessShaderComponent : ShaderComponent {};

	template<typename T, typename ...Args>
	static inline GBufferShaderComponent makeGBufferShaderComponent(Args && ... args) {
		return GBufferShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	template<typename T, typename ...Args>
	static inline LightingShaderComponent makeLightingShaderComponent(Args && ... args) {
		return LightingShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	template<typename T, typename ...Args>
	static inline PostLightingShaderComponent makePostLightingShaderComponent(Args && ... args) {
		return PostLightingShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	template<typename T, typename ...Args>
	static inline PostProcessShaderComponent makePostProcessShaderComponent(Args && ... args) {
		return PostProcessShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	struct DepthMapComponent {
		putils::gl::FrameBuffer fbo;
		putils::gl::Texture texture;
		int size = -1;
	};

	struct CSMComponent {
		putils::gl::FrameBuffer fbo;
		putils::gl::Texture textures[KENGINE_MAX_CSM_COUNT];
		int size = -1;
	};

	struct DepthCubeComponent {
		putils::gl::FrameBuffer fbo;
		putils::gl::Texture texture;
		int size = -1;
	};

	struct ShadowMapShaderComponent {};

	struct ShadowCubeShaderComponent {};
}

#define refltype kengine::GBufferShaderComponent 
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::LightingShaderComponent 
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::PostLightingShaderComponent 
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::PostProcessShaderComponent 
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::DepthMapComponent 
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::CSMComponent 
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::DepthCubeComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::ShadowMapShaderComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::ShadowCubeShaderComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype
