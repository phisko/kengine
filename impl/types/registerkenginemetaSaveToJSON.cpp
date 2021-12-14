#include "helpers/registerTypeHelper.hpp"
#include "meta/SaveToJSON.hpp"

namespace kengine::impl::types{
	void registerkenginemetaSaveToJSON() noexcept {
		kengine::registerComponents<kengine::meta::SaveToJSON>();

	}
}