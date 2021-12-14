#include "helpers/registerTypeHelper.hpp"
#include "meta/DetachFrom.hpp"

namespace kengine::impl::types{
	void registerkenginemetaDetachFrom() noexcept {
		kengine::registerComponents<kengine::meta::DetachFrom>();

	}
}