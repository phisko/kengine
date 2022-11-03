#include "EntityIterator.hpp"

// kengine
#include "Entity.hpp"

// kengine impl
#include "Mutex.hpp"

namespace kengine::impl {
	EntityIterator & EntityIterator::operator++() noexcept {
		++index;
		ReadLock l(state->_entitiesMutex);
		while (index < state->_entities.size() && (state->_entities[index].data.mask == 0 || !state->_entities[index].data.active))
			++index;
		return *this;
	}

	bool EntityIterator::operator!=(const EntityIterator & rhs) const noexcept {
		// Use `<` as it will only be compared with `end()`, and there is a risk that new entities have been added since `end()` was called
		return index < rhs.index;
	}

	bool EntityIterator::operator==(const EntityIterator & rhs) const noexcept {
		return !(*this != rhs);
	}

	Entity EntityIterator::operator*() const noexcept {
		ReadLock l(state->_entitiesMutex);
		return { index, state->_entities[index].data.mask };
	}
}
