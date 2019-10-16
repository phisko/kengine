#pragma once

#include "System.hpp"

namespace kengine {
	class ImGuiOverlaySystem : public kengine::System<ImGuiOverlaySystem> {
	public:
		ImGuiOverlaySystem(kengine::EntityManager & em);
		~ImGuiOverlaySystem();

		void init() const;
		void execute() noexcept final;

	private:
		kengine::EntityManager & _em;
	};
}