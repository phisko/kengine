#pragma once

#include "opengl/Program.hpp"

namespace kengine {
	class EntityManager;
}

namespace kengine::Shaders {
	class Highlight : public putils::gl::Program {
	public:
		Highlight(kengine::EntityManager & em);

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override;
		void run(const Parameters & params) override;

	private:
		kengine::EntityManager & _em;

	public:
		putils::gl::Uniform<glm::vec3> _viewPos;
		putils::gl::Uniform<putils::Point2f> _screenSize;

		putils::gl::Uniform<float> _entityID;
		putils::gl::Uniform<putils::NormalizedColor> _highlightColor;
		putils::gl::Uniform<float> _intensity;

		pmeta_get_attributes(
			pmeta_reflectible_attribute_private(&Highlight::_viewPos),
			pmeta_reflectible_attribute_private(&Highlight::_screenSize),

			pmeta_reflectible_attribute_private(&Highlight::_entityID),
			pmeta_reflectible_attribute_private(&Highlight::_highlightColor),
			pmeta_reflectible_attribute_private(&Highlight::_intensity)
		);
	};
}