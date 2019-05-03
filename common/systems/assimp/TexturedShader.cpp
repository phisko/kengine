#include "TexturedShader.hpp"

#include "components/TransformComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/TexturedModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "helpers/ShaderHelper.hpp"

#include "AssImpHelper.hpp"

#include "TexturedShaderSrc.hpp"
static_assert(KENGINE_ASSIMP_BONE_INFO_PER_VERTEX == 4, "This shader assumes only 4 bones per vertex");

namespace kengine {
	void TexturedShader::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<TexturedShader>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::TexturedShader::frag, GL_FRAGMENT_SHADER }
		));

		_diffuseTextureID = firstTextureID;
		putils::gl::setUniform(texture_diffuse, _diffuseTextureID);

		_specularTextureID = _diffuseTextureID + 1;
		putils::gl::setUniform(texture_specular, _specularTextureID);
	}

	void TexturedShader::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();

		glEnable(GL_DEPTH_TEST);

		putils::gl::setUniform(this->view, view);
		putils::gl::setUniform(this->proj, proj);

		for (const auto &[e, textured, modelComp, transform, skeleton] : _em.getEntities<TexturedModelComponent, kengine::ModelComponent, kengine::TransformComponent3f, SkeletonComponent>()) {
			putils::gl::setUniform(this->entityID, (float)e.id);

			AssImpHelper::Locations locations;
			locations.model = this->model;
			locations.bones = this->bones;
			locations.diffuseTextureID = _diffuseTextureID;
			locations.specularTextureID = _specularTextureID;

			AssImpHelper::drawModel(_em, modelComp, transform, skeleton, true, locations);
		}

		glDisable(GL_DEPTH_TEST);
	}
}