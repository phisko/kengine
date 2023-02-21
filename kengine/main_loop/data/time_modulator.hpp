#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::main_loop {
	//! putils reflect all
	struct time_modulator {
		float factor = 1.f;
	};
}

#include "time_modulator.rpp"