#pragma once

#ifdef _WIN32

// kengine
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

namespace kengine::core::log::visual_studio {
	DECLARE_KENGINE_SYSTEM_CREATOR(KENGINE_CORE_LOG_VISUAL_STUDIO_EXPORT, system)
}

#endif