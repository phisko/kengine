#include "helpers/registerTypeHelper.hpp"
#include "data/NameComponent.hpp"

namespace kengine::types{
	void registerkengineNameComponent() noexcept {
		kengine::registerComponents<kengine::NameComponent>();

	}
}