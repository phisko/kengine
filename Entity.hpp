#pragma once

#include <cstddef>
#include "Component.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine {
	class Entity {
	public:
		using ID = size_t;
		static constexpr auto INVALID_ID = detail::INVALID;

		Entity(size_t id = detail::INVALID) : id(id) {}

		~Entity() = default;
		Entity(const Entity &) = default;
		Entity & operator=(const Entity &) = default;
		Entity(Entity &&) = default;
		Entity & operator=(Entity &&) = default;

	public:
		template<typename T>
		T & get() {
			assert("No such component" && has<T>());
			return Component<T>::get(id);
		}

		template<typename T>
		const T & get() const {
			assert("No such component" && has<T>());
			return Component<T>::get(id);
		}

		template<typename T>
		bool has() const {
			return componentMask & getMask<T>();
		}

		template<typename T, typename ... Args>
		T & attach(Args && ... args) {
			componentMask |= getMask<T>();
			return get<T>();
		}

		template<typename T>
		Entity & operator+=(T && obj) {
			attach<T>() = FWD(obj);
			return *this;
		}

		template<typename T>
		void detach() {
			assert("No such component" && has<T>());
			componentMask &= ~getMask<T>();
		}

		size_t id;
		long long componentMask = 0;

	private:
		template<typename T>
		long long getMask() const {
			static const auto id = Component<T>::id();
			assert("You are using too many component types" && id < sizeof(componentMask) * 8);
			return 1ll << id;
		}

	public:
		pmeta_get_class_name(Entity);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&Entity::id)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}