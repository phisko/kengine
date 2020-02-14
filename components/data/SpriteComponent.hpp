#pragma once

#include "reflection.hpp"
#include "OnScreenComponent.hpp"

namespace kengine {
	struct SpriteComponent2D : OnScreenComponent { // UI element
		putils_reflection_class_name(SpriteComponent2D);
	};

	struct SpriteComponent3D { // Ground feedback and such
		putils_reflection_class_name(SpriteComponent3D);
	};
}