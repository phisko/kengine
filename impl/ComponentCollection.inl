#include "ComponentCollection.hpp"
#include "kengine.hpp"

#define TCollection ComponentCollection<Comps...>
#define TIterator ComponentIterator<Comps...>

namespace kengine::impl {
	template<typename ... Comps>
	TIterator TCollection::begin() const noexcept {
		ReadLock l(state->_archetypesMutex);

		size_t archetypeIndex = 0;
		for (auto & archetype : state->_archetypes) {
			if (archetype.matches<Comps...>()) {
				ReadLock l(archetype.mutex);
				ReadLock l2(state->_entitiesMutex);

				size_t entityIndex = 0;
				for (const auto entityID : archetype.entities) {
					if (state->_entities[entityID].active)
						return { archetypeIndex, entityIndex };
					++entityIndex;
				}
			}
			++archetypeIndex;
		}

		return end();
	}

	template<typename ... Comps>
	TIterator TCollection::end() const noexcept {
		return { (size_t)-1, 0 };
	}

	template<typename ... Comps>
	TIterator & TIterator::operator++() noexcept {
		++currentEntity;
		auto & archetype = state->_archetypes[currentType];

		{
			ReadLock l(archetype.mutex);
			if (currentEntity < archetype.entities.size())
			{
				ReadLock l(state->_entitiesMutex);
				if (state->_entities[archetype.entities[currentEntity]].active)
					return *this;
			}
		}

		currentEntity = 0;
		{
			ReadLock l(state->_archetypesMutex);
			for (++currentType; currentType < state->_archetypes.size(); ++currentType)
				if (state->_archetypes[currentType].matches<Comps...>())
					break;
			if (currentType >= state->_archetypes.size())
				currentType = (size_t)-1;
		}

		return *this;
	}

	template<typename ... Comps>
	bool TIterator::operator!=(const TIterator & rhs) const noexcept {
		// Use `<` as it will only be compared with `end()`, and there is a risk that new entities have been added since `end()` was called
		return currentType < rhs.currentType || currentEntity < rhs.currentEntity;
	}

	template<typename ... Comps>
	bool TIterator::operator==(const TIterator & rhs) const noexcept {
		return !(*this != rhs);
	}

	template<typename ... Comps>
	template<typename T>
	T & TIterator::get(Entity & e) noexcept {
		if constexpr (is_not<T>()) {
			static T ret;
			return ret;
		}
		else
			return e.get<T>();
	};

	template<typename ... Comps>
	std::tuple<Entity, Comps &...> TIterator::operator*() const noexcept {
		ReadLock l(state->_archetypesMutex);
		const auto & archetype = state->_archetypes[currentType];

		ReadLock l2(archetype.mutex);
		Entity e(archetype.entities[currentEntity], archetype.mask);
		return std::make_tuple(e, std::ref(get<Comps>(e))...);
	}
}

#undef TCollection
#undef TIterator
