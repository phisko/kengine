#pragma once

#include <glm/glm.hpp>
#include "Point.hpp"
#include "opengl/Program.hpp"
#include "DepthCubeSrc.hpp"

namespace kengine {
	class Entity;
	struct DirLightComponent;
	struct SpotLightComponent;
	struct PointLightComponent;
}

namespace kengine::Shaders {
	class ShadowMapShader : public putils::gl::Program {
	public:
		ShadowMapShader(bool usesGBuffer = false, const char * name = "") : Program(usesGBuffer, name) {}
		virtual ~ShadowMapShader() {}

		virtual void drawToTexture(GLuint texture, const glm::mat4 & lightSpaceMatrix, const Parameters & params) {}
		void run(const Parameters & params) override {}

		virtual void run(Entity & e, DirLightComponent & light, const Parameters & params);
		virtual void run(Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params);

	private:
		template<typename T, typename Func>
		void runImpl(T & depthMap, Func && draw, const Parameters & params);
	};

	class ShadowCubeShader : public putils::gl::Program,
		public Shaders::src::DepthCube::Geom::Uniforms,
		public Shaders::src::DepthCube::Frag::Uniforms
	{
	public:
		ShadowCubeShader(bool usesGBuffer = false, const char * name = "") : Program(usesGBuffer, name) {}
		virtual ~ShadowCubeShader() {}

		void run(const Parameters & params) override {}
		virtual void run(Entity & e, PointLightComponent & light, const putils::Point3f & pos, float radius, const Parameters & params);

		virtual void drawObjects(const Parameters & params) {}

#pragma region Uniforms
	protected:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

	public:
		putils_reflection_attributes(
			putils_reflection_attribute_private(&ShadowCubeShader::_proj),
			putils_reflection_attribute_private(&ShadowCubeShader::_view),
			putils_reflection_attribute_private(&ShadowCubeShader::_model)
		);

		putils_reflection_parents(
			putils_reflection_type(Shaders::src::DepthCube::Geom::Uniforms),
			putils_reflection_type(Shaders::src::DepthCube::Frag::Uniforms)
		);
#pragma endregion Uniforms
	};
}