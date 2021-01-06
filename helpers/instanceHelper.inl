#include "instanceHelper.hpp"

namespace kengine::instanceHelper {
	template<typename Comp>
	bool modelHas(const InstanceComponent & instance) noexcept {
		kengine_assert(instance.model != INVALID_ID);
		const auto model = entities.get(instance.model);
		return model.has<Comp>();
	}

	template<typename Comp>
	bool modelHas(const Entity & instance) noexcept {
		return modelHas<Comp>(instance.get<InstanceComponent>());
	}

	template<typename Comp>
	const Comp & getModel(const InstanceComponent & instance) noexcept {
		kengine_assert(instance.model != INVALID_ID);
		const auto model = entities.get(instance.model);
		return model.get<Comp>();
	}

	template<typename Comp>
	const Comp & getModel(const Entity & e) noexcept {
		return getModel<Comp>(e.get<InstanceComponent>());
	}

	template<typename Comp>
	const Comp * tryGetModel(const InstanceComponent & instance) noexcept {
		kengine_assert(instance.model != INVALID_ID);
		const auto model = entities.get(instance.model);
		return model.tryGet<Comp>();
	}

	template<typename Comp>
	const Comp * tryGetModel(const Entity & e) noexcept {
		return tryGetModel<Comp>(e.get<InstanceComponent>());
	}
}
