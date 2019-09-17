#pragma once

#include "not_serializable.hpp"

namespace kengine {
	struct TextureModelComponent : not_serializable {
		GLuint texture;
	};
}