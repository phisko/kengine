#pragma once

#include "System.hpp"

namespace kengine {
	class ImGuiEntitySelectorSystem : public kengine::System<ImGuiEntitySelectorSystem> {
	public:
		ImGuiEntitySelectorSystem(kengine::EntityManager & em);
		void onLoad(const char * directory) noexcept final;
		
	private:
		kengine::EntityManager & _em;
	};
}