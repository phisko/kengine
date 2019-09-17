#pragma once

#include "System.hpp"

#include <unordered_map>
#include "components/AdjustableComponent.hpp"

namespace kengine {
	class ImGuiAdjustableSystem : public kengine::System<ImGuiAdjustableSystem, kengine::packets::RegisterEntity> {
		using string = AdjustableComponent::string;

	public:
		ImGuiAdjustableSystem(kengine::EntityManager & em);

		void onLoad(const char * directory) noexcept final;
		void onSave(const char * directory) noexcept final;

		void handle(const packets::RegisterEntity & p);

	private:
		kengine::EntityManager & _em;
		struct PointerSave {
			bool * bPtr;
			int * iPtr;
			float * dPtr;
		};
		std::unordered_map<string, PointerSave> _pointerSaves;
	};
}