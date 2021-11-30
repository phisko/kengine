#pragma once

#include <string>
#include <vector>

namespace kengine {
	struct AnimationFilesComponent {
		std::vector<std::string> files;
	};
}

#define refltype kengine::AnimationFilesComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(files)
	);
};
#undef refltype