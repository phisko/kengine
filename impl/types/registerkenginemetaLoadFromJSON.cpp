#include "helpers/registerTypeHelper.hpp"
#include "meta/LoadFromJSON.hpp"

namespace kengine::impl::types{
	void registerkenginemetaLoadFromJSON() noexcept {
		kengine::registerComponents<kengine::meta::LoadFromJSON>();

	}
}