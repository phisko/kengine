#pragma once

#include "opengl/Program.hpp"
#include "Entity.hpp"

namespace kengine {
	class EntityManager;
	struct DirLightComponent;
	struct SpotLightComponent;
}

namespace kengine::Shaders {
	struct DepthMapComponent {
		GLuint fbo = -1;
		GLuint texture;
		pmeta_get_class_name(DepthMapComponent);
	};

	class ShadowMap : public putils::gl::Program {
	public:
		ShadowMap(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override {}
		void run(kengine::Entity & e, DirLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight);
		void run(kengine::Entity & e, SpotLightComponent & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight);

	private:
		template<typename T>
		void runImpl(kengine::Entity & e, T & light, const putils::Point3f & pos, size_t screenWidth, size_t screenHeight);

	public:
		GLint proj;
		GLint view;
		GLint model;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ShadowMap::proj),
			pmeta_reflectible_attribute(&ShadowMap::view),
			pmeta_reflectible_attribute(&ShadowMap::model)
		);
	};

}