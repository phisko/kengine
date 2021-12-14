#include "helpers/registerTypeHelper.hpp"
#include "meta/Size.hpp"

namespace kengine::impl::types{
	void registerkenginemetaSize() noexcept {
		kengine::registerComponents<kengine::meta::Size>();

	}
}