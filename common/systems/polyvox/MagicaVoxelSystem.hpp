#pragma once

#include "System.hpp"

namespace kengine {
	class MagicaVoxelSystem : public kengine::System<MagicaVoxelSystem, kengine::packets::RegisterEntity> {
	public:
		MagicaVoxelSystem(kengine::EntityManager & em);

		void handle(kengine::packets::RegisterEntity p);
		void loadModel(kengine::Entity & e);
		void setModel(kengine::Entity & e);

	private:
		kengine::EntityManager & _em;
	};
}
