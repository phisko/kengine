#include "helpers/registerTypeHelper.hpp"
#include "data/HighlightComponent.hpp"

namespace kengine::types{
	void registerkengineHighlightComponent() noexcept {
		kengine::registerComponents<kengine::HighlightComponent>();

	}
}