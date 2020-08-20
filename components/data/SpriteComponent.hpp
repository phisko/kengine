#pragma once

#include "reflection.hpp"
#include "OnScreenComponent.hpp"

namespace kengine {
	struct SpriteComponent2D : OnScreenComponent { // UI element
	};

	struct SpriteComponent3D { // Ground feedback and such
	};
}

#define refltype kengine::SpriteComponent2D
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::OnScreenComponent)
	);
};
#undef refltype

#define refltype kengine::SpriteComponent3D
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype
