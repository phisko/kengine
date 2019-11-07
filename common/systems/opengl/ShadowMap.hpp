#pragma once

#include "opengl/Program.hpp"
#include "Entity.hpp"

#include "components/ShaderComponent.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
	struct SpotLightComponent;
}

namespace kengine::Shaders {
	class ShadowMap : public ShadowMapShader {
	public:
		ShadowMap(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override {}
		void run(kengine::Entity & e, DirLightComponent & light, const Parameters & params) override;
		void run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, const Parameters & params) override;

	private:
		template<typename T, typename Func>
		void runImpl(T & depthMap, Func && draw, const Parameters & params);

		void drawToTexture(GLuint texture);

		kengine::EntityManager & _em;

	public:
		putils::gl::Uniform<glm::mat4> _proj;
		putils::gl::Uniform<glm::mat4> _view;
		putils::gl::Uniform<glm::mat4> _model;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&ShadowMap::_proj),
			pmeta_reflectible_attribute_private(&ShadowMap::_view),
			pmeta_reflectible_attribute_private(&ShadowMap::_model)
		);
	};

}