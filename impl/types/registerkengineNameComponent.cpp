#include "helpers/registerTypeHelper.hpp"
#include "data/NameComponent.hpp"

namespace kengine::impl::types{
	void registerkengineNameComponent() noexcept {
		kengine::registerComponents<kengine::NameComponent>();

	}
}