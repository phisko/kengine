#pragma once

#include <vector>
#include "System.hpp"

namespace kengine {
	class ImGuiEntityEditorSystem : public kengine::System<ImGuiEntityEditorSystem> {
	public:
		ImGuiEntityEditorSystem(kengine::EntityManager & em);

	private:
		kengine::EntityManager & _em;
	};
}
