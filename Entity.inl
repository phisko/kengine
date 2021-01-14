#include "Entity.hpp"
#include "impl/Component.hpp"
#include "helpers/assertHelper.hpp"

namespace kengine {
	template<typename T>
	T & Entity::get() noexcept {
		kengine_assert_with_message(has<T>(), "No such component");
		return impl::Component<T>::get(id);
	}

	template<typename T>
	const T & Entity::get() const noexcept {
		kengine_assert_with_message(has<T>(), "No such component");
		return impl::Component<T>::get(id);
	}

	template<typename T>
	bool Entity::has() const noexcept {
		return componentMask.test(impl::Component<T>::id());
	}

	template<typename T>
	T * Entity::tryGet() noexcept {
		if (has<T>())
			return &get<T>();
		return nullptr;
	}

	template<typename T>
	const T * Entity::tryGet() const noexcept {
		if (has<T>())
			return &get<T>();
		return nullptr;
	}

	template<typename T>
	Entity & Entity::operator+=(T && comp) noexcept {
		attach<T>(FWD(comp));
		return *this;
	}

	template<typename T>
	T & kengine::Entity::attach() noexcept {
		if (!has<T>()) {
			static const auto component = impl::Component<T>::id();
			componentMask.set(component, true);
			impl::addComponent(id, component);
		}
		return get<T>();
	}

	template<typename T>
	void kengine::Entity::attach(T && comp) noexcept {
		using Comp = std::decay_t<T>;

		impl::Component<Comp>::get(id) = FWD(comp);
		if (!has<Comp>()) {
			static const auto component = impl::Component<Comp>::id();
			componentMask.set(component, true);
			impl::addComponent(id, component);
		}
	}

	template<typename T>
	void kengine::Entity::detach() noexcept {
		auto & comp = get<T>();
		comp.~T();
		new (&comp) T;

		static const auto component = impl::Component<T>::id();
		componentMask.set(component, false);
		impl::removeComponent(id, component);
	}
}