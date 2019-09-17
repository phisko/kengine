#pragma once

#include "System.hpp"
#include "packets/Collision.hpp"

namespace kengine {
	class BulletSystem : public kengine::System<BulletSystem, kengine::packets::RemoveEntity, kengine::packets::QueryPosition> {
	public:
		BulletSystem(EntityManager & em);
		void onLoad(const char * directory) noexcept override;
		void execute() override;

		void handle(packets::RemoveEntity p);
		void handle(const packets::QueryPosition & p);

	private:
		EntityManager & _em;
	};
}