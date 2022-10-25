#include "helpers/registerTypeHelper.hpp"
#include "data/HighlightComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineHighlightComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::HighlightComponent'");
		kengine::registerComponents<kengine::HighlightComponent>();
	}
}