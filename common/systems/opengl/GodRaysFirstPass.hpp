#pragma once

#include "opengl/Program.hpp"

namespace kengine { class EntityManager; }

namespace kengine::Shaders {
	class GodRaysFirstPass : public putils::gl::Program {
	public:
		GodRaysFirstPass(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override {}
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::mat4 & model, const glm::vec3 & color);

		auto getTexture() const { return _texture; }

	private:
		kengine::EntityManager & _em;

		GLuint _fbo;
		GLuint _texture;

	public:
		GLint proj;
		GLint view;
		GLint model;

		GLint color;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GodRaysFirstPass::proj),
			pmeta_reflectible_attribute(&GodRaysFirstPass::view),
			pmeta_reflectible_attribute(&GodRaysFirstPass::model),

			pmeta_reflectible_attribute(&GodRaysFirstPass::color)
		);
	};
}
