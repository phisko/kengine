#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace Color {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::NormalizedColor> _color;

				pmeta_get_attributes(
					pmeta_reflectible_attribute_private(&Uniforms::_color)
				);
			};
		}
	}
}