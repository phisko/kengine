#pragma once

#include <cstddef>
#include "Component.hpp"
#include "reflection/Reflectible.hpp"

#ifndef KENGINE_COMPONENT_COUNT
# define KENGINE_COMPONENT_COUNT 64
#endif

static constexpr auto Components = KENGINE_COMPONENT_COUNT;

namespace kengine {
	class Entity {
	public:
		using ID = size_t;
		static constexpr auto INVALID_ID = detail::INVALID;

	public:
		Entity(size_t id = detail::INVALID) : id(id) {
			std::fill_n(componentIds, Components, detail::INVALID);
		}

		~Entity() = default;
		Entity(const Entity &) = default;
		Entity & operator=(const Entity &) = default;
		Entity(Entity &&) = default;
		Entity & operator=(Entity &&) = default;

	public:
		template<typename T>
		T & get() {
			const auto index = getIndex<T>();
			assert("No such component" && index != detail::INVALID);
			return Component<T>::get(componentIds[index]);
		}

		template<typename T>
		const T & get() const {
			const auto index = getIndex<T>();
			assert("No such component" && index != detail::INVALID);
			return Component<T>::get(componentIds[index]);
		}

		template<typename T>
		bool has() const {
			return componentIds[getIndex<T>()] != detail::INVALID;
		}

		template<typename T>
		T & attach() {
			componentIds[getIndex<T>()] = Component<T>::alloc();
			return get<T>();
		}

		template<typename T>
		void detach() {
			const auto index = getIndex<T>();
			assert("No such component" && index != detail::INVALID);
			Component<T>::release(componentIds[index]);
			componentIds[index] = detail::INVALID;
		}

		size_t id;

	private:
		template<typename T>
		size_t getIndex() const {
			const auto index = Component<T>::id();
			assert("You are using too many component types" && index < Components);
			return index;
		}

		size_t componentIds[Components];

	public:
		pmeta_get_class_name(Entity);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Entity::id)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}