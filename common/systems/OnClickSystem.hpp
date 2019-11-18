#pragma once

#include "System.hpp"

namespace kengine {
	class OnClickSystem : public kengine::System<OnClickSystem> {
	public:
		OnClickSystem(kengine::EntityManager& em);

	private:
		kengine::EntityManager & _em;
	};
}
