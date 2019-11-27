#pragma once

#include "System.hpp"

namespace kengine {
	class KinematicSystem : public System<KinematicSystem> {
	public:
		KinematicSystem(EntityManager & em);

		void execute() override;

	private:
		EntityManager & _em;
	};
}