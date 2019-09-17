#pragma once

#include "System.hpp"
#include "packets/AddImGuiTool.hpp"

namespace kengine {
	class ImGuiOverlaySystem : public kengine::System<ImGuiOverlaySystem, kengine::packets::AddImGuiTool> {
	public:
		ImGuiOverlaySystem(kengine::EntityManager & em);
		~ImGuiOverlaySystem();

		void init() const;
		void execute() noexcept final;

		void handle(kengine::packets::AddImGuiTool p);

	private:
		kengine::EntityManager & _em;
	};
}