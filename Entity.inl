#include "Entity.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/profilingHelper.hpp"

// impl
#include "impl/Component.hpp"

namespace kengine {
	template<typename T>
	T & Entity::get() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert_with_message(has<T>(), "No such component");
		return impl::Component<T>::get(id);
	}

	template<typename T>
	const T & Entity::get() const noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert_with_message(has<T>(), "No such component");
		return impl::Component<T>::get(id);
	}

	template<typename T>
	bool Entity::has() const noexcept {
		KENGINE_PROFILING_SCOPE;
		return componentMask.test(impl::Component<T>::id());
	}

	template<typename T>
	T * Entity::tryGet() noexcept {
		KENGINE_PROFILING_SCOPE;

		if (has<T>())
			return &get<T>();
		return nullptr;
	}

	template<typename T>
	const T * Entity::tryGet() const noexcept {
		KENGINE_PROFILING_SCOPE;

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
	T & Entity::attach() noexcept {
		KENGINE_PROFILING_SCOPE;

        using Comp = std::decay_t<T>;

		auto & ret = impl::Component<Comp>::set(id);
		if (!has<Comp>()) {
			const auto component = impl::Component<Comp>::id();
			componentMask.set(component, true);
			impl::addComponent(id, component);
		}

		return ret;
	}

	template<typename T>
	std::decay_t<T> & Entity::attach(T && value) noexcept {
		KENGINE_PROFILING_SCOPE;

		using Comp = std::decay_t<T>;

		auto & ret = impl::Component<Comp>::set(id, FWD(value));
		if (!has<Comp>()) {
			const auto component = impl::Component<Comp>::id();
			componentMask.set(component, true);
			impl::addComponent(id, component);
		}

		return ret;
	}

	template<typename T>
	void kengine::Entity::detach() noexcept {
		KENGINE_PROFILING_SCOPE;

        impl::Component<T>::metadata().reset(id);

		const auto component = impl::Component<T>::id();
		componentMask.set(component, false);
		impl::removeComponent(id, component);
	}
}