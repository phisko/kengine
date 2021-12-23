#include "helpers/registerTypeHelper.hpp"
#include "data/ModelSkeletonComponent.hpp"

namespace kengine::types{
	void registerkengineModelSkeletonComponent() noexcept {
		kengine::registerComponents<kengine::ModelSkeletonComponent>();

	}
}