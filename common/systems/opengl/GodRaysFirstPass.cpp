#include "GodRaysFirstPass.hpp"
#include "RAII.hpp"
#include "Shapes.hpp"

#include "EntityManager.hpp"

namespace kengine::Shaders {
	GodRaysFirstPass::GodRaysFirstPass(kengine::EntityManager & em)
		: _em(em)
	{
	}


	void GodRaysFirstPass::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<GodRaysFirstPass>(putils::make_vector(
			ShaderDescription{ "shaders/3d.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/color.frag", GL_FRAGMENT_SHADER }
		));

		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		texturesToDebug.push_back(Texture{ "God Rays first pass", _texture });
	}

	void GodRaysFirstPass::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::mat4 & model, const glm::vec3 & color) {
		BindFramebuffer _(_fbo);
		use();

		glClear(GL_COLOR_BUFFER_BIT);

		putils::gl::setUniform(this->model, model);
		putils::gl::setUniform(this->view, view);
		putils::gl::setUniform(this->proj, proj);
		putils::gl::setUniform(this->color, color);
		shapes::drawSphere();

		putils::gl::setUniform(this->color, glm::vec3(0.f));
		drawObjects(this->model);
	}
}
