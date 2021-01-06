#pragma once

#include "reflection.hpp"

namespace kengine {
	class Entity;
}

namespace kengine::impl {
	struct EntityIterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = Entity;
		using reference = value_type &;
		using pointer = value_type *;
		using difference_type = size_t;

		EntityIterator & operator++() noexcept;
		bool operator!=(const EntityIterator & rhs) const noexcept;
		bool operator==(const EntityIterator & rhs) const noexcept;
		Entity operator*() const noexcept;

		EntityIterator(size_t index) noexcept : index(index) {}
		EntityIterator(const EntityIterator &) noexcept = default;
		EntityIterator & operator=(const EntityIterator & rhs) noexcept {
			index = rhs.index;
			return *this;
		}

		size_t index;
	};
}
