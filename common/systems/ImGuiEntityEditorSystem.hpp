#pragma once

#include <vector>
#include "System.hpp"

namespace kengine {
	class ImGuiEntityEditorSystem : public kengine::System<ImGuiEntityEditorSystem> {
	public:
		ImGuiEntityEditorSystem(kengine::EntityManager & em) : System(em), _em(em) {
			onLoad("");
		}

		void onLoad(const char * directory) noexcept final;

	private:
		kengine::EntityManager & _em;
	};
}
