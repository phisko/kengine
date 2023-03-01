#pragma once

// reflection
#include "putils/reflection.hpp"

// kengine render
#include "kengine/render/data/on_screen.hpp"

namespace kengine::render {
	//! putils reflect all
	//! parents: [kengine::render::on_screen]
	struct sprite_2d : on_screen { // UI element
	};

	//! putils reflect all
	struct sprite_3d { // Ground feedback and such
	};
}

#include "sprite.rpp"