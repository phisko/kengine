#pragma once

#include <vector>

namespace kengine {
	// Used only by shaders, built from a ModelDataComponent
	template<typename Mesh>
	struct SystemSpecificModelComponent {
		std::vector<Mesh> meshes;
	};
}