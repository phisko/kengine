#pragma once

namespace kengine::data {
	//! putils reflect all
	struct god_rays {
		float scattering = .1f;
		int nb_steps = 10;
		float default_step_length = 25.f;
		float intensity = 2.5f;
	};
}

#include "god_rays.rpp"