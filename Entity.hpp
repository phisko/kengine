#pragma once

#include "impl/ID.hpp"
#include "impl/ComponentMask.hpp"
#include "reflection.hpp"

namespace kengine {
	class Entity {
	public:
		EntityID id;
		impl::ComponentMask componentMask = 0;

		template<typename T> 
		T & get() noexcept;

		template<typename T> 
		const T & get() const noexcept;

		template<typename T>
		bool has() const noexcept;

		template<typename T>
		T * tryGet() noexcept;

		template<typename T>
		const T * tryGet() const noexcept;

		template<typename T>
		Entity & operator+=(T && comp) noexcept;

		template<typename T>
		T & attach() noexcept;

		template<typename T>
		void attach(T && rhs) noexcept;

		template<typename T>
		void detach() noexcept;
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

#include "Entity.inl"
