#pragma once

#include "RecastComponent.hpp"

#include <DebugDraw.h>
#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class RecastDebugShader : public duDebugDraw, public putils::gl::Program {
	public:
#pragma region ctor / dtor
		RecastDebugShader(EntityManager & em);
		~RecastDebugShader() = default;
#pragma endregion ctor / dtor

#pragma region Program
	public:
		void init(size_t firstTexture) final;
		void run(const Parameters & params) final;
#pragma endregion Program

#pragma region duDebugDraw
	public:
		void depthMask(bool state) final {}
		void texture(bool state) final {}

		void begin(duDebugDrawPrimitives prim, float size = 1.f) final;

		void vertex(const float * pos, unsigned int color) final { vertex(pos[0], pos[1], pos[2], color); }
		void vertex(const float x, const float y, const float z, unsigned int color) final;

		void vertex(const float * pos, unsigned int color, const float * uv) final { vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]); }
		void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) final;

		void end() final;
#pragma endregion duDebugDraw

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<putils::NormalizedColor> _color;
#pragma endregion Uniforms

#pragma region Attributes
	private:
		EntityManager & _em;

		struct Vertex {
			float pos[3];
			float color[4];
		};
		putils_reflection_friend(Vertex);

		std::vector<Vertex> _currentMesh;
		GLenum _currentVertexType;
		GLuint _vao;
		GLuint _vbo;
#pragma endregion Attributes
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
