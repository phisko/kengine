#pragma once

#include "System.hpp"
#include "packets/RegisterComponentEditor.hpp"

namespace kengine {
	class ImGuiEntitySelectorSystem : public kengine::System<ImGuiEntitySelectorSystem, packets::RegisterComponentEditor> {
	public:
		ImGuiEntitySelectorSystem(kengine::EntityManager & em);
		void onLoad(const char * directory) noexcept final;
		void handle(packets::RegisterComponentEditor p);
		
	private:
		kengine::EntityManager & _em;
		std::vector<packets::RegisterComponentEditor> _comps;
	};
}