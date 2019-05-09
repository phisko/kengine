#pragma once

#include "System.hpp"
#include "packets/EntityInPixel.hpp"

namespace kengine {
	class EntityManager;

	class AssImpSystem : public kengine::System<AssImpSystem, kengine::packets::RegisterEntity, kengine::packets::GetEntityInPixel> {
	public:
		AssImpSystem(kengine::EntityManager & em);

		void onLoad(const char *) noexcept override;

		void handle(kengine::packets::RegisterEntity p);
		void handle(kengine::packets::GetEntityInPixel p);

		void execute() override;

	private:
		kengine::EntityManager & _em;
	};
}
