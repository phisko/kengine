#include "helpers/registerTypeHelper.hpp"
#include "meta/Count.hpp"

namespace kengine::types{
	void registerkenginemetaCount() noexcept {
		kengine::registerComponents<kengine::meta::Count>();

	}
}