#pragma once

#include "System.hpp"

namespace kengine {
	class ImGuiEntitySelectorSystem : public kengine::System<ImGuiEntitySelectorSystem> {
	public:
		ImGuiEntitySelectorSystem(kengine::EntityManager & em);
		
	private:
		kengine::EntityManager & _em;
	};
}