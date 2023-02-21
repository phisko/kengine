#pragma once

// entt
#include <entt/entity/fwd.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/function.hpp"

#ifndef KENGINE_FUNCTION_MAX_SIZE
#define KENGINE_FUNCTION_MAX_SIZE 64
#endif

#ifndef KENGINE_ENTITY_ITERATOR_FUNC_SIZE
#define KENGINE_ENTITY_ITERATOR_FUNC_SIZE 64
#endif

namespace kengine {
	// Helper type when a callback is needed as an iterator
	using entity_iterator_signature = void(entt::handle);
	using entity_iterator_func = putils::function<entity_iterator_signature, KENGINE_ENTITY_ITERATOR_FUNC_SIZE>;

	template<typename>
	struct base_function;

	template<typename Ret, typename... Args>
	struct base_function<Ret(Args...)> {
		using base = base_function<Ret(Args...)>;
		using callable = putils::function<Ret(Args...), KENGINE_FUNCTION_MAX_SIZE>;

		callable func = nullptr;

		bool operator==(std::nullptr_t) const noexcept { return func == nullptr; }
		bool operator!=(std::nullptr_t) const noexcept { return func != nullptr; }

		Ret operator()(Args... args) const noexcept { return func(args...); }
		Ret call(Args... args) const noexcept { return func(args...); }
	};
}

template<typename Ret, typename... Args>
#define refltype kengine::base_function<Ret(Args...)>
putils_reflection_info_template {
	putils_reflection_attributes(
		putils_reflection_attribute(func)
	);
};
#undef refltype

#define kengine_function_reflection_info \
	putils_reflection_info { \
		putils_reflection_class_name; \
		putils_reflection_parents( \
			putils_reflection_type(refltype::base) \
		); \
	};
