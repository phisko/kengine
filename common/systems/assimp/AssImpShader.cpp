#include "AssImpShader.hpp"

#include "components/TransformComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "systems/opengl/shaders/shaders.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

#include "AssImpHelper.hpp"

#include "AssImpShaderSrc.hpp"
static_assert(KENGINE_ASSIMP_BONE_INFO_PER_VERTEX == 4, "This shader assumes only 4 bones per vertex");

namespace kengine {
	void AssImpShader::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<AssImpShader>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::TexturedShader::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ kengine::Shaders::src::ApplyTransparency::frag, GL_FRAGMENT_SHADER }
		));

		_diffuseTextureID = firstTextureID;
		putils::gl::setUniform(texture_diffuse, _diffuseTextureID);

		// _specularTextureID = _diffuseTextureID + 1;
		// putils::gl::setUniform(texture_specular, _specularTextureID);
	}

	void AssImpShader::run(const Parameters & params) {
		use();

		putils::gl::setUniform(this->view, params.view);
		putils::gl::setUniform(this->proj, params.proj);

		for (const auto &[e, textured, graphics, transform, skeleton] : _em.getEntities<AssImpObjectComponent, GraphicsComponent, TransformComponent3f, SkeletonComponent>()) {
			if (graphics.model == Entity::INVALID_ID)
				return;

			putils::gl::setUniform(this->entityID, (float)e.id);
			putils::gl::setUniform(this->color, graphics.color);

			AssImpHelper::Locations locations;
			locations.model = this->model;
			locations.bones = this->bones;
			locations.hasTexture = this->hasTexture;
			locations.diffuseTextureID = _diffuseTextureID;
			locations.specularTextureID = _specularTextureID;
			locations.specularColor = this->specularColor;
			locations.diffuseColor = this->diffuseColor;

			AssImpHelper::drawModel(_em, graphics, transform, skeleton, true, locations);
		}
	}
}