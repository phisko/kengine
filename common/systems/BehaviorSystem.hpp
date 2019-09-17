#pragma once

#include "System.hpp"
#include "EntityManager.hpp"
#include "components/BehaviorComponent.hpp"

namespace kengine {
	class BehaviorSystem : public kengine::System<BehaviorSystem>
	{
	public:
		BehaviorSystem(kengine::EntityManager & em) : System(em), _em(em) {}

	public:
		void execute() final {
			for (const auto & [e, comp] : _em.getEntities<kengine::BehaviorComponent>())
				comp.func();
		}

	private:
		kengine::EntityManager & _em;
	};
}