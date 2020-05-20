#pragma once

#include "RecastComponent.hpp"

#include <RecastDump.h>
#include <DebugDraw.h>
#include <RecastDebugDraw.h>
#include <DetourDebugDraw.h>

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class RecastDebugShader : public duDebugDraw, public putils::gl::Program {
	public:
		RecastDebugShader(EntityManager & em);
		~RecastDebugShader() = default;

	public: // Program members
		void init(size_t firstTexture) final;
		void run(const Parameters & params) final;

	public: // duDebugDraw members
		void depthMask(bool state) final {}
		void texture(bool state) final {}

		void begin(duDebugDrawPrimitives prim, float size = 1.f) final;

		void vertex(const float * pos, unsigned int color) final { vertex(pos[0], pos[1], pos[2], color); }
		void vertex(const float x, const float y, const float z, unsigned int color) final;

		void vertex(const float * pos, unsigned int color, const float * uv) final { vertex(pos[0], pos[1], pos[2], color, uv[0], uv[1]); }
		void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) final;

		void end() final;

	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<putils::NormalizedColor> _color;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&RecastDebugShader::_model),
			putils_reflection_attribute_private(&RecastDebugShader::_view),
			putils_reflection_attribute_private(&RecastDebugShader::_proj),
			putils_reflection_attribute_private(&RecastDebugShader::_viewPos)
		);

	private:
		EntityManager & _em;

		struct Vertex {
			float pos[3];
			float color[4];

			putils_reflection_attributes(
				putils_reflection_attribute(&Vertex::pos),
				putils_reflection_attribute(&Vertex::color)
			);
		};

		std::vector<Vertex> _currentMesh;
		GLenum _currentVertexType;
		GLuint _vao;
		GLuint _vbo;
	};
}
