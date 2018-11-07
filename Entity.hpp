#pragma once

#include <cstddef>
#include "Component.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine {
	class EntityManager;

	class EntityView {
	public:
		using ID = size_t;
		using Mask = long long;
		static constexpr auto INVALID_ID = detail::INVALID;
		static constexpr auto MAX_COMPONENTS = sizeof(Mask) * 8;

		EntityView(size_t id) : id(id) {}

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
			return componentMask & getMask<T>();
		}

		ID id;
		Mask componentMask = 0;

	protected:
		template<typename T>
		Mask getMask() const {
			static const auto id = Component<T>::id();
			assert("You are using too many component types" && id < MAX_COMPONENTS);
			return 1ll << id;
		}

	public:
		pmeta_get_class_name(EntityView);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&EntityView::id),
			pmeta_reflectible_attribute(&EntityView::componentMask)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};

	class Entity : public EntityView {
	public:
		Entity(size_t id = detail::INVALID, EntityManager * manager = nullptr) : EntityView(id), manager(manager) {}
		~Entity() = default;
		Entity(const Entity &) = default;
		Entity & operator=(const Entity & rhs) {
			id = rhs.id;
			componentMask = rhs.componentMask;
			return *this;
		}

		template<typename T>
		Entity & operator+=(T && comp) {
			attach<T>() = FWD(comp);
			return *this;
		}

		template<typename T>
		T & attach();

		template<typename T>
		void detach();

	private:
		EntityManager * manager;
	};
}

#include "EntityManager.hpp"

template<typename T>
T & kengine::Entity::attach() {
	const auto oldMask = componentMask ;
	componentMask |= getMask<T>();
	manager->updateMask(*this, oldMask);
	return get<T>();
}

template<typename T>
void kengine::Entity::detach() {
	assert("No such component" && has<T>());
	const auto oldMask = componentMask;
	componentMask &= ~getMask<T>();
	manager->updateMask(*this, oldMask);
}

