#pragma once

#include <memory>

#include "not_serializable.hpp"
#include "putils/opengl/Program.hpp"

namespace kengine {
	class Entity;

	struct ShaderComponent {
		std::unique_ptr<putils::gl::Program> shader;
		bool enabled = true;
	};

	struct GBufferShaderComponent : ShaderComponent, kengine::not_serializable {
		pmeta_get_class_name(GBufferShaderComponent);
	};
	struct LightingShaderComponent : ShaderComponent, kengine::not_serializable {
		pmeta_get_class_name(LightingShaderComponent);
	};
	struct PostProcessShaderComponent : ShaderComponent, kengine::not_serializable {
		pmeta_get_class_name(PostProcessShaderComponent);
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
	static inline PostProcessShaderComponent makePostProcessShaderComponent(Args && ... args) {
		return PostProcessShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	struct DirLightComponent;
	struct SpotLightComponent;
	struct PointLightComponent;

	struct DepthMapComponent {
		GLuint fbo = -1;
		GLuint texture;
		pmeta_get_class_name(DepthMapComponent);
	};
	class ShadowMapShader : public putils::gl::Program {
	public:
		ShadowMapShader(bool usesGBuffer = false, const char * name = "") : Program(usesGBuffer, name) {}
		virtual void run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) = 0;
		virtual void run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) = 0;
	};
	struct ShadowMapShaderComponent : kengine::not_serializable {
		pmeta_get_class_name(ShadowMapShaderComponent);
	};

	struct DepthCubeComponent {
		GLuint fbo = -1;
		GLuint texture;
		pmeta_get_class_name(DepthCubeComponent);
	};
	class ShadowCubeShader : public putils::gl::Program {
	public:
		ShadowCubeShader(bool usesGBuffer = false, const char * name = "") : Program(usesGBuffer, name) {}
		virtual void run(kengine::Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, size_t screenWidth, size_t screenHeight) = 0;
	};
	struct ShadowCubeShaderComponent : kengine::not_serializable {
		pmeta_get_class_name(ShadowCubeShaderComponent);
	};
}