#pragma once

#include <vector>
#include "Mutex.hpp"
#include "ComponentMask.hpp"
#include "ID.hpp"

namespace kengine::impl {
	struct Archetype {
		ComponentMask mask;
		std::vector<ID> entities;
		bool sorted = true;
		mutable Mutex mutex;

		Archetype(ComponentMask mask, EntityID firstEntity) noexcept;
		Archetype() noexcept = default;
		Archetype(Archetype &&) noexcept;
		Archetype & operator=(Archetype &&) noexcept;
		Archetype(const Archetype &) noexcept;
		Archetype & operator=(const Archetype &) noexcept;

		void add(EntityID id) noexcept;
		void remove(EntityID id) noexcept;
		void sort() noexcept;

		template<typename ... Comps>
		bool matches() noexcept;
	};
}

#include "Archetype.inl"