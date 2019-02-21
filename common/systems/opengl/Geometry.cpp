#include "Geometry.hpp"
#include "RAII.hpp"

namespace kengine::Shaders {
	void Geometry::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<Geometry>(putils::make_vector(
			ShaderDescription{ "shaders/geometry.vert", GL_VERTEX_SHADER },
			ShaderDescription{ "shaders/geometry.frag", GL_FRAGMENT_SHADER }
		));
	}

	void Geometry::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();

		Enable __t(GL_DEPTH_TEST);
		// Enable __c(GL_CULL_FACE);

		putils::gl::setUniform(this->view, view);
		putils::gl::setUniform(this->proj, proj);
		putils::gl::setUniform(viewPos, camPos);

		drawObjects(model);
	}
}