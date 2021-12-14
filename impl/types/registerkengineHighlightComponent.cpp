#include "helpers/registerTypeHelper.hpp"
#include "data/HighlightComponent.hpp"

namespace kengine::impl::types{
	void registerkengineHighlightComponent() noexcept {
		kengine::registerComponents<kengine::HighlightComponent>();

	}
}