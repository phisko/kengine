#pragma once

#include "opengl/Uniform.hpp"

namespace kengine::Shaders::src {
	namespace Color {
		namespace Frag {
			extern const char * glsl;

			struct Uniforms {
				putils::gl::Uniform<putils::NormalizedColor> _color;
			};
		}
	}
}

#define refltype kengine::Shaders::src::Color::Frag::Uniforms
putils_reflection_info{
	putils_reflection_attributes(
		putils_reflection_attribute_private(_color)
	);
};
#undef refltype