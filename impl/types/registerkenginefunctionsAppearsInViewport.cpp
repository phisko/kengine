#include "helpers/registerTypeHelper.hpp"
#include "functions/AppearsInViewport.hpp"

namespace kengine::impl::types{
	void registerkenginefunctionsAppearsInViewport() noexcept {
		kengine::registerComponents<kengine::functions::AppearsInViewport>();

	}
}