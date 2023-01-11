#pragma once

// reflection
#include "putils/reflection.hpp"

// kengine data
#include "kengine/data/on_screen.hpp"

namespace kengine::data {
	//! putils reflect all
	//! parents: [kengine::data::on_screen]
	struct sprite_2d : data::on_screen { // UI element
	};

	//! putils reflect all
	struct sprite_3d { // Ground feedback and such
	};
}

#include "sprite.reflection.hpp"