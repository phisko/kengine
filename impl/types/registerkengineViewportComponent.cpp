#include "helpers/registerTypeHelper.hpp"
#include "data/ViewportComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineViewportComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ViewportComponent'");
		kengine::registerComponents<kengine::ViewportComponent>();

	}
}