#include "helpers/registerTypeHelper.hpp"
#include "meta/Has.hpp"

namespace kengine::types{
	void registerkenginemetaHas() noexcept {
		kengine::registerComponents<kengine::meta::Has>();

	}
}