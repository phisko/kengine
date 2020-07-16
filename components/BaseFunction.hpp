#pragma once

#include "reflection.hpp"
#include "function.hpp"

#ifndef KENGINE_FUNCTION_MAX_SIZE
# define KENGINE_FUNCTION_MAX_SIZE 64
#endif

#ifndef KENGINE_ENTITY_ITERATOR_FUNC_SIZE
# define KENGINE_ENTITY_ITERATOR_FUNC_SIZE 64
#endif

namespace kengine {
	// Helper type when a callback is needed as an iterator
	class Entity;
	using EntityIteratorSignature = void(Entity &);
	using EntityIteratorFunc = putils::function<EntityIteratorSignature, KENGINE_ENTITY_ITERATOR_FUNC_SIZE>;

	namespace functions {
		template<typename>
		struct BaseFunction;

		template<typename Ret, typename ... Args>
		struct BaseFunction<Ret(Args...)> {
			putils::function<Ret(Args...), KENGINE_FUNCTION_MAX_SIZE> func;

			bool operator==(nullptr_t) const { return func == nullptr; }
			bool operator!=(nullptr_t) const { return func != nullptr; }

			Ret operator()(Args... args) const { return func(args...); }
			Ret call(Args ... args) const { return func(args...); }

			putils_reflection_attributes(
				putils_reflection_attribute(&BaseFunction::func)
			);
		};
	}
}

