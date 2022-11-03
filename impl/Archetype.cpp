#include "Archetype.hpp"

// stl
#include <algorithm>

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine::impl {
	Archetype::Archetype(ComponentMask mask, EntityID firstEntity) noexcept
		: mask(mask), entities({ firstEntity })
	{}

	Archetype::Archetype(Archetype && rhs) noexcept
		: mask(rhs.mask), sorted(rhs.sorted)
	{
		KENGINE_PROFILING_SCOPE;
		WriteLock l(rhs.mutex);
		entities = std::move(rhs.entities);
	}

	Archetype & Archetype::operator=(Archetype && rhs) noexcept {
		KENGINE_PROFILING_SCOPE;
		mask = rhs.mask;
		sorted = rhs.sorted;
		WriteLock l(rhs.mutex);
		entities = std::move(rhs.entities);
		return *this;
	}

	Archetype::Archetype(const Archetype & rhs) noexcept
		: mask(rhs.mask), sorted(rhs.sorted)
	{
		KENGINE_PROFILING_SCOPE;
		ReadLock l(rhs.mutex);
		entities = rhs.entities;
	}

	Archetype & Archetype::operator=(const Archetype & rhs) noexcept {
		KENGINE_PROFILING_SCOPE;
		mask = rhs.mask;
		sorted = rhs.sorted;
		ReadLock l(rhs.mutex);
		entities = rhs.entities;
		return *this;
	}


	void Archetype::add(EntityID id) noexcept {
		KENGINE_PROFILING_SCOPE;
		WriteLock l(mutex);
		entities.push_back(id);
		sorted = false;
	}

	void Archetype::remove(EntityID id) noexcept {
		KENGINE_PROFILING_SCOPE;
		WriteLock l(mutex);
		if (!sorted)
			sort();
		const auto it = std::lower_bound(entities.begin(), entities.end(), id);
		std::swap(*it, entities.back());
		entities.pop_back();
		sorted = false;
	}

	void Archetype::sort() noexcept {
		KENGINE_PROFILING_SCOPE;
		std::sort(entities.begin(), entities.end(), std::less<EntityID>());
		sorted = true;
	}
}
