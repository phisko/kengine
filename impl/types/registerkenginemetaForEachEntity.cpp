#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"

namespace kengine::impl::types{
	void registerkenginemetaForEachEntity() noexcept {
		kengine::registerComponents<kengine::meta::ForEachEntity>();

	}
}