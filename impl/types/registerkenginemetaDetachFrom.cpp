#include "helpers/registerTypeHelper.hpp"
#include "meta/DetachFrom.hpp"

namespace kengine::types{
	void registerkenginemetaDetachFrom() noexcept {
		kengine::registerComponents<kengine::meta::DetachFrom>();

	}
}