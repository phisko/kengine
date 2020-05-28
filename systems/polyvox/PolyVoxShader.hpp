#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;

	class PolyVoxShader : public putils::gl::Program {
	public:
		PolyVoxShader(EntityManager & em);

		void init(size_t firstTextureID) override;
		void run(const Parameters & params) override;

#pragma region Uniforms
	public:
		putils::gl::Uniform<glm::mat4> _model;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::vec3> _viewPos;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _color;

		putils_reflection_attributes(
			putils_reflection_attribute_private(&PolyVoxShader::_model),
			putils_reflection_attribute_private(&PolyVoxShader::_view),
			putils_reflection_attribute_private(&PolyVoxShader::_proj),
			putils_reflection_attribute_private(&PolyVoxShader::_viewPos),

			putils_reflection_attribute_private(&PolyVoxShader::_entityID),
			putils_reflection_attribute_private(&PolyVoxShader::_color)
		);
#pragma endregion Uniforms

	private:
		EntityManager & _em;
	};
}