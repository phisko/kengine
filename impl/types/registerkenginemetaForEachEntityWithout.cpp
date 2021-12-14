#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"

namespace kengine::impl::types{
	void registerkenginemetaForEachEntityWithout() noexcept {
		kengine::registerComponents<kengine::meta::ForEachEntityWithout>();

	}
}