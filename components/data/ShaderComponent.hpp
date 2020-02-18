#pragma once

#include <memory>

#include "putils/opengl/Program.hpp"
#include "data/LightComponent.hpp"

namespace kengine {
	class Entity;

	struct ShaderComponent {
		std::unique_ptr<putils::gl::Program> shader;
		bool enabled = true;
	};

	struct GBufferShaderComponent : ShaderComponent {
		putils_reflection_class_name(GBufferShaderComponent);
	};
	struct LightingShaderComponent : ShaderComponent {
		putils_reflection_class_name(LightingShaderComponent);
	};
	struct PostLightingShaderComponent : ShaderComponent {
		putils_reflection_class_name(PostLightingShaderComponent);
	};

	struct PostProcessShaderComponent : ShaderComponent {
		putils_reflection_class_name(PostProcessShaderComponent);
	};

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
		GLuint fbo = (GLuint)-1;
		GLuint texture = (GLuint)-1;
		int size = -1;
		putils_reflection_class_name(DepthMapComponent);
	};

	struct CSMComponent {
		GLuint fbo = (GLuint)-1;
		GLuint textures[KENGINE_CSM_COUNT];
		int size = -1;
		putils_reflection_class_name(CSMComponent);
	};

	struct DepthCubeComponent {
		GLuint fbo = (GLuint)-1;
		GLuint texture = (GLuint)-1;
		int size = -1;
		putils_reflection_class_name(DepthCubeComponent);
	};

	struct ShadowMapShaderComponent {
		putils_reflection_class_name(ShadowMapShaderComponent);
	};

	struct ShadowCubeShaderComponent {
		putils_reflection_class_name(ShadowCubeShaderComponent);
	};
}