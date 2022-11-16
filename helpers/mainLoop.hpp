#pragma once

namespace kengine::mainLoop {
	KENGINE_CORE_EXPORT void run() noexcept;

	namespace timeModulated {
		KENGINE_CORE_EXPORT void run() noexcept;
	}
}