#include "AssImpShadowMap.hpp"
#include "EntityManager.hpp"

#include "components/SkeletonComponent.hpp"
#include "components/TexturedModelComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"

#include "helpers/LightHelper.hpp"
#include "helpers/ShaderHelper.hpp"

#include "TexturedShaderSrc.hpp"
#include "AssImpHelper.hpp"

namespace kengine {
	void AssImpShadowMap::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<AssImpShadowMap>(putils::make_vector(
			ShaderDescription{ src::TexturedShader::vert, GL_VERTEX_SHADER }
		));

		putils::gl::setUniform(proj, glm::mat4(1.f));
	}

	template<typename T>
	void AssImpShadowMap::runImpl(kengine::Entity & e, T & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		if (!e.has<DepthMapComponent>())
			return;

		glViewport(0, 0, KENGINE_SHADOW_MAP_SIZE, KENGINE_SHADOW_MAP_SIZE);

		const auto & depthMap = e.get<DepthMapComponent>();
		ShaderHelper::BindFramebuffer __f(depthMap.fbo);
		ShaderHelper::Enable __e(GL_DEPTH_TEST);

		use();
		putils::gl::setUniform(view, LightHelper::getLightSpaceMatrix(light, { pos.x, pos.y, pos.z }, screenWidth, screenHeight));
		glCullFace(GL_FRONT);

		for (const auto &[e, textured, modelComp, transform, skeleton] : _em.getEntities<TexturedModelComponent, kengine::ModelComponent, kengine::TransformComponent3f, SkeletonComponent>()) {
			AssImpHelper::Locations locations;
			locations.model = this->model;
			locations.bones = this->bones;

			AssImpHelper::drawModel(_em, modelComp, transform, skeleton, false, locations);
		}

		glCullFace(GL_BACK);
		glViewport(0, 0, screenWidth, screenHeight);
	}

	void AssImpShadowMap::run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}
	void AssImpShadowMap::run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight) {
		runImpl(e, light, pos, screenWidth, screenHeight);
	}
}
