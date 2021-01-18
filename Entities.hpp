#pragma once

#include "Entity.hpp"
#include "impl/EntityIterator.hpp"
#include "impl/ComponentCollection.hpp"

namespace kengine {
	class Entities {
	public:
		template<typename Func> // Func: EntityCreator
		Entity create(Func && postCreate) noexcept;
		template<typename Func>
		Entity operator+=(Func && postCreate) noexcept;

		void remove(Entity e) noexcept;
		void remove(EntityID id) noexcept;
		void operator-=(Entity e) noexcept;
		void operator-=(EntityID id) noexcept;

		Entity get(EntityID id) noexcept;
		Entity operator[](EntityID id) noexcept;

		void setActive(Entity e, bool active) noexcept;
		void setActive(EntityID id, bool active) noexcept;

		template<typename ... Comps>
		impl::ComponentCollection<Comps...> with() const noexcept;

		using value_type = impl::EntityIterator::value_type;
		using reference = impl::EntityIterator::reference;
		using iterator = impl::EntityIterator;

		impl::EntityIterator begin() const noexcept;
		impl::EntityIterator end() const noexcept;
	};
}

#include "Entities.inl"