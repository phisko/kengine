#include "helpers/registerTypeHelper.hpp"
#include "meta/MatchString.hpp"

namespace kengine::impl::types{
	void registerkenginemetaMatchString() noexcept {
		kengine::registerComponents<kengine::meta::MatchString>();

	}
}