#pragma once

#include "BaseFunction.hpp"

#ifndef KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE
# define KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE 64
#endif

namespace kengine {
	namespace meta {
		// Note that `member` is a pointer to the actual member, not a member pointer
		// e.g.	("value", &value, putils::meta::type<float>::index)
		// NOT	("value", &Class::value, putils::meta::type<float>::index)
		using AttributeIteratorSignature = void(const char * name, const void * member, putils::meta::type_index memberType);
		using AttributeIteratorFunc = putils::function<AttributeIteratorSignature, KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE>;

		struct ForEachAttribute : functions::BaseFunction<
			void(Entity & e, const AttributeIteratorFunc & func)
		> {
			putils_reflection_class_name(ForEachAttribute);
		};
	}
}