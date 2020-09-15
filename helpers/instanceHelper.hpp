#pragma once

#include "EntityManager.hpp"
#include "data/InstanceComponent.hpp"
#include "assertHelper.hpp"

namespace kengine::instanceHelper {
	template<typename Comp>
	bool modelHas(EntityManager & em, const Entity & instance);
	template<typename Comp>
	bool modelHas(EntityManager & em, const InstanceComponent & instance);

	template<typename Comp>
	const Comp & getModel(EntityManager & em, const Entity & instance);
	template<typename Comp>
	const Comp & getModel(EntityManager & em, const InstanceComponent & instance);

	template<typename Comp>
	const Comp & tryGetModel(EntityManager & em, const Entity & instance);
	template<typename Comp>
	const Comp & tryGetModel(EntityManager & em, const InstanceComponent & instance);
}

// impl
namespace kengine::instanceHelper {
	template<typename Comp>
	bool modelHas(EntityManager & em, const InstanceComponent & instance) {
		kengine_assert(em, instance.model != Entity::INVALID_ID);
		const auto model = em.getEntity(instance.model);
		return model.has<Comp>();
	}

	template<typename Comp>
	bool modelHas(EntityManager & em, const Entity & instance) {
		return modelHas<Comp>(em, e.get<InstanceComponent>());
	}

	template<typename Comp>
	const Comp & getModel(EntityManager & em, const InstanceComponent & instance) {
		kengine_assert(em, instance.model != Entity::INVALID_ID);
		const auto model = em.getEntity(instance.model);
		return model.get<Comp>();
	}

	template<typename Comp>
	const Comp & getModel(EntityManager & em, const Entity & e) {
		return getModel<Comp>(em, e.get<InstanceComponent>());
	}

	template<typename Comp>
	const Comp * tryGetModel(EntityManager & em, const InstanceComponent & instance) {
		kengine_assert(em, instance.model != Entity::INVALID_ID);
		const auto model = em.getEntity(instance.model);
		return model.tryGet<Comp>();
	}

	template<typename Comp>
	const Comp * tryGetModel(EntityManager & em, const Entity & e) {
		return tryGetModel<Comp>(em, e.get<InstanceComponent>());
	}
}