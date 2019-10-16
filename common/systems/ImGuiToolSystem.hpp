#pragma once

#include "System.hpp"

namespace kengine {
	class ImGuiToolSystem : public System<ImGuiToolSystem, packets::RegisterEntity> {
	public:
		ImGuiToolSystem(EntityManager & em);

		void onLoad(const char *) noexcept override;
		void handle(packets::RegisterEntity p) noexcept;

	private:
		EntityManager & _em;
	};
}