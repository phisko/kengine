#pragma once

#ifdef _WIN32

// kengine helpers
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

namespace kengine::systems {
	DECLARE_KENGINE_SYSTEM_CREATOR(KENGINE_LOG_LOG_VISUAL_STUDIO_EXPORT, log_visual_studio)
}

#endif