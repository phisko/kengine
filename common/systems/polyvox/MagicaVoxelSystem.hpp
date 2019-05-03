#pragma once

#include "System.hpp"

namespace kengine {
	class MagicaVoxelSystem : public kengine::System<MagicaVoxelSystem, kengine::packets::RegisterEntity> {
	public:
		MagicaVoxelSystem(kengine::EntityManager & em);

		void onLoad(const char *) noexcept override;

		void handle(kengine::packets::RegisterEntity p);

	private:
		kengine::EntityManager & _em;
	};
}
