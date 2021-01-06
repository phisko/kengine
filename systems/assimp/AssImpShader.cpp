#include "AssImpShader.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/SkeletonComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "AssImpHelper.hpp"

#include "systems/opengl/shaders/ApplyTransparencySrc.hpp"
#include "AssImpShaderSrc.hpp"
static_assert(KENGINE_ASSIMP_BONE_INFO_PER_VERTEX == 4, "This shader assumes only 4 bones per vertex");

namespace kengine {
	void AssImpShader::init(size_t firstTextureID) noexcept {
		initWithShaders<AssImpShader>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::TexturedShader::frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ opengl::shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_diffuseTextureID = firstTextureID;
		_texture_diffuse = _diffuseTextureID;
		_specularTextureID = _diffuseTextureID + 1;
		_texture_specular = _specularTextureID;
	}

	void AssImpShader::run(const Parameters & params) noexcept {
		use();

		_view = params.view;
		_proj = params.proj;

		for (const auto &[e, assimp, instance, graphics, transform, skeleton] : entities.with<AssImpObjectComponent, InstanceComponent, GraphicsComponent, TransformComponent, SkeletonComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
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

			AssImpHelper::drawModel(instance, transform, skeleton, true, uniforms);
		}
	}
}