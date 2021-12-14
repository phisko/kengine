#include "helpers/registerTypeHelper.hpp"
#include "meta/Get.hpp"

namespace kengine::impl::types{
	void registerkenginemetaGet() noexcept {
		kengine::registerComponents<kengine::meta::Get>();

	}
}