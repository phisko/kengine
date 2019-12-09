#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace Color {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::NormalizedColor> _color;

				putils_reflection_attributes(
					putils_reflection_attribute_private(&Uniforms::_color)
				);
			};
		}
	}
}