#pragma once

#include <vector>
#include "System.hpp"
#include "packets/RegisterComponentEditor.hpp"

namespace kengine {
	class ImGuiEntityEditorSystem : public kengine::System<ImGuiEntityEditorSystem, kengine::packets::RegisterComponentEditor> {
	public:
		ImGuiEntityEditorSystem(kengine::EntityManager & em) : System(em), _em(em) {
			onLoad("");
		}

		void onLoad(const char * directory) noexcept final;
		void handle(kengine::packets::RegisterComponentEditor p);

	private:
		kengine::EntityManager & _em;
		std::vector<packets::RegisterComponentEditor> _comps;
	};
}
