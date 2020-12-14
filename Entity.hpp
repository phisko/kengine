#pragma once

#include <assert.h>
#include <bitset>
#include <cstddef>
#include "Component.hpp"
#include "reflection.hpp"

#ifndef KENGINE_COMPONENT_COUNT
# define KENGINE_COMPONENT_COUNT 64
#endif

namespace kengine {
	class EntityManager;

	class EntityView {
	public:
		using ID = size_t;
		using Mask = std::bitset<KENGINE_COMPONENT_COUNT>;
		static constexpr auto INVALID_ID = detail::INVALID;

		EntityView(ID id = INVALID_ID, Mask componentMask = 0) : id(id), componentMask(componentMask) {}

		~EntityView() = default;
		EntityView(const EntityView &) = default;
		EntityView & operator=(const EntityView & rhs) = default;

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
			return componentMask.test(getId<T>());
		}

		template<typename T>
		T * tryGet() {
			if (has<T>())
				return &get<T>();
			return nullptr;
		}

		template<typename T>
		const T * tryGet() const {
			if (has<T>())
				return &get<T>();
			return nullptr;
		}

		ID id;
		Mask componentMask = 0;

	protected:
		template<typename T>
		size_t getId() const {
			static const auto id = Component<T>::id();
			assert("You are using too many component types." && id < KENGINE_COMPONENT_COUNT);
			return id;
		}
	};

	class Entity : public EntityView {
	public:
		Entity(ID id = detail::INVALID, Mask componentMask = 0, EntityManager * manager = nullptr) : EntityView(id, componentMask), manager(manager) {}
		~Entity() = default;
		Entity(const Entity &) = default;
		Entity & operator=(const Entity & rhs) = default;

		template<typename T>
		Entity & operator+=(T && comp) {
			attach<T>(FWD(comp));
			return *this;
		}

		template<typename T>
		T & attach();
		template<typename T>
		void attach(T && rhs);

		template<typename T>
		void detach();

	private:
		EntityManager * manager;
	};
}

#define refltype kengine::Entity
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(id),
		putils_reflection_attribute(componentMask)
	);
};
#undef refltype
