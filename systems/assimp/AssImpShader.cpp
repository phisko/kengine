#include "AssImpShader.hpp"

#include "data/TransformComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/OpenGLModelComponent.hpp"
#include "data/SkeletonComponent.hpp"

#include "systems/opengl/shaders/ApplyTransparencySrc.hpp"

#include "systems/opengl/shaders/ShaderHelper.hpp"

#include "AssImpHelper.hpp"

#include "AssImpShaderSrc.hpp"
static_assert(KENGINE_ASSIMP_BONE_INFO_PER_VERTEX == 4, "This shader assumes only 4 bones per vertex");

namespace kengine {
	void AssImpShader::init(size_t firstTextureID) {
		initWithShaders<AssImpShader>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::TexturedShader::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ Shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_diffuseTextureID = firstTextureID;
		_texture_diffuse = _diffuseTextureID;

		// _specularTextureID = _diffuseTextureID + 1;
		// _texture_specular = _specularTextureID;
	}

	void AssImpShader::run(const Parameters & params) {
		use();

		_view = params.view;
		_proj = params.proj;

		for (const auto &[e, textured, graphics, transform, skeleton] : _em.getEntities<AssImpObjectComponent, GraphicsComponent, TransformComponent, SkeletonComponent>()) {
			if (graphics.model == Entity::INVALID_ID)
				continue;
			if (!ShaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_entityID = (float)e.id;
			_color = graphics.color;

			AssImpHelper::Uniforms uniforms;
			uniforms.model = _model;
			uniforms.bones = _bones;
			uniforms.hasTexture = _hasTexture;
			uniforms.diffuseTextureID = _diffuseTextureID;
			uniforms.specularTextureID = _specularTextureID;
			uniforms.specularColor = _specularColor;
			uniforms.diffuseColor = _diffuseColor;

			AssImpHelper::drawModel(_em, graphics, transform, skeleton, true, uniforms);
		}
	}
}