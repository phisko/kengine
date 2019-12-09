#pragma once

#include "reflection.hpp"

namespace kengine {
	struct SpriteComponent2D { // UI element
		putils_reflection_class_name(SpriteComponent2D);
	};

	struct SpriteComponent3D { // Ground feedback and such
		putils_reflection_class_name(SpriteComponent3D);
	};
}