#pragma once

#include "reflection.hpp"

namespace kengine {
	struct SpriteComponent2D { // UI element
		pmeta_get_class_name(SpriteComponent2D);
	};

	struct SpriteComponent3D { // Ground feedback and such
		pmeta_get_class_name(SpriteComponent3D);
	};
}