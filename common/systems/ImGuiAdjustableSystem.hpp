#pragma once

#include "System.hpp"

#include <unordered_map>
#include "components/AdjustableComponent.hpp"

namespace kengine {
	class ImGuiAdjustableSystem : public kengine::System<ImGuiAdjustableSystem, kengine::packets::RegisterEntity> {
		using string = AdjustableComponent::string;

	public:
		ImGuiAdjustableSystem(kengine::EntityManager & em);
		~ImGuiAdjustableSystem();

		void handle(const packets::RegisterEntity & p);

	private:
		kengine::EntityManager & _em;
	};
}