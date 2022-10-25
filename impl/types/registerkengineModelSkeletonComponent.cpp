#include "helpers/registerTypeHelper.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineModelSkeletonComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelSkeletonComponent'");
		kengine::registerComponents<kengine::ModelSkeletonComponent>();
	}
}