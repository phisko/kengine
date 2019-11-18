#pragma once

#include "System.hpp"

namespace kengine {
	class EntityManager;

	class PolyVoxSystem : public kengine::System<PolyVoxSystem> {
	public:
		PolyVoxSystem(kengine::EntityManager & em);
		void execute() noexcept override;

	private:
		kengine::EntityManager & _em;
	};
}
