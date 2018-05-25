#pragma once

#include "System.hpp"
#include "components/BehaviorComponent.hpp"

namespace kengine {
	class BehaviorSystem : public kengine::System<BehaviorSystem>
	{
	public:
		BehaviorSystem(kengine::EntityManager & em) : _em(em) {}

	public:
		void execute() final {
			for (const auto go : _em.getGameObjects<kengine::BehaviorComponent>())
				go->getComponent<kengine::BehaviorComponent>().func();
		}

	private:
		kengine::EntityManager & _em;
	};
}