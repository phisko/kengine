#pragma once

#include "Entity.hpp"
#include "reflection.hpp"

namespace kengine::impl {
	template<typename ... Comps>
	struct ComponentIterator {
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::tuple<Entity, Comps & ...> &;
		using reference = value_type &;
		using pointer = std::tuple<Entity, Comps & ...> *;
		using difference_type = size_t;

		ComponentIterator & operator++() noexcept;
		bool operator!=(const ComponentIterator & rhs) const noexcept;
		bool operator==(const ComponentIterator & rhs) const noexcept;
        operator bool() const noexcept;

		std::tuple<Entity, Comps &...> operator*() const noexcept;

		size_t currentType;
		size_t currentEntity;

	private:
		template<typename T>
		static T & get(Entity & e) noexcept;
	};

	template<typename ... Comps>
	struct ComponentCollection {
		using value_type = typename ComponentIterator<Comps...>::value_type;
		using reference = typename ComponentIterator<Comps...>::reference;
		using iterator = ComponentIterator<Comps...>;

		ComponentIterator<Comps...> begin() const noexcept;
		ComponentIterator<Comps...> end() const noexcept;
		size_t size() const noexcept;
	};
}

template<typename ... Comps>
#define refltype kengine::impl::ComponentCollection<Comps...>
putils_reflection_info_template{
	template<typename T>
	static constexpr std::string getClassName() {
		return putils::reflection::get_class_name<T>();
	}

	static constexpr auto class_name = "kengineComponentCollection" + (getClassName<Comps>() + ...);
};
#undef refltype

#include "ComponentCollection.inl"