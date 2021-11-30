#pragma once

#include "RecastComponent.hpp"

#include <DebugDraw.h>
#include "opengl/Program.hpp"

namespace kengine {
	class RecastDebugShader : public duDebugDraw, public putils::gl::Program {
	public:
		RecastDebugShader() noexcept;
		~RecastDebugShader() noexcept = default;

		// Program
	public:
		void init(size_t firstTexture) noexcept final;
		void run(const Parameters & params) noexcept final;

		// duDebugDraw
	public:
		void depthMask(bool state) final {}
		void texture(bool state) final {}

		void begin(duDebugDrawPrimitives prim, float size = 1.f) final;

		void vertex(const float * pos, unsigned int color) final { vertex(pos[0], pos[1], pos[2], color); }
		void vertex(const float x, const float y, const float z, unsigned int color) final;

		void vertex(const float * pos, unsigned int color, const float * uv) final { vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]); }
		void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) final;

		void end() final;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<putils::NormalizedColor> _color;
#pragma endregion Uniforms

	private:
		struct Vertex {
			float pos[3];
			float color[4];
		};
		putils_reflection_friend(Vertex);

		std::vector<Vertex> _currentMesh;
		GLenum _currentVertexType;
		GLuint _vao;
		GLuint _vbo;
	};
}

#define refltype kengine::RecastDebugShader 
putils_reflection_info {
	putils_reflection_attributes(
		putils_reflection_attribute_private(_model),
		putils_reflection_attribute_private(_view),
		putils_reflection_attribute_private(_proj),
		putils_reflection_attribute_private(_viewPos)
	);
};
#undef refltype

#define refltype kengine::RecastDebugShader::Vertex
putils_reflection_info {
	putils_reflection_attributes(
		putils_reflection_attribute(pos),
		putils_reflection_attribute(color)
	);
};
#undef refltype
