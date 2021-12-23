#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"

namespace kengine::types{
	void registerkenginemetaForEachEntity() noexcept {
		kengine::registerComponents<kengine::meta::ForEachEntity>();

	}
}