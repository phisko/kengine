#pragma once

#include "System.hpp"

namespace kengine {
	class OnClickSystem : public kengine::System<OnClickSystem> {
	public:
		OnClickSystem(kengine::EntityManager& em);

		void onLoad(const char *) noexcept override;

	private:
		kengine::EntityManager & _em;
	};
}
