#pragma once

#include <vector>
#include "ID.hpp"
#include "Mutex.hpp"

namespace kengine::impl {
	struct ComponentMetadata {
		ID id = INVALID_ID;
		virtual void reset(ID id) noexcept = 0;
		virtual ~ComponentMetadata() noexcept = default;
	};

	template<typename Comp>
	struct Metadata : ComponentMetadata {
		using Chunk = std::vector<Comp>;

		std::vector<Chunk> chunks;
		mutable Mutex _mutex;

		void reset(ID id) noexcept final;
	};
}