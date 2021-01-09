#pragma once

#include "ID.hpp"
#include "Mutex.hpp"
#include "ComponentSettings.hpp"

namespace kengine::impl {
	struct ComponentMetadata {
		ID id = INVALID_ID;
		virtual void reset(ID id) noexcept = 0;
		virtual ~ComponentMetadata() noexcept = default;
	};

	template<typename Comp>
	struct Metadata : ComponentMetadata {
		componentSettings::map<Comp> _map;
		Mutex _mutex;
		void reset(ID id) noexcept final;
	};
}