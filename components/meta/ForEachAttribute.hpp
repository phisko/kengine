#pragma once

#include "BaseFunction.hpp"

#ifndef KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE
# define KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE 64
#endif

namespace kengine {
	namespace meta {
		struct AttributeInfo {
			const char * name;
			void * member; // pointer to the actual member, not a member pointer
			size_t size;
			putils::meta::type_index type;
		};

		using AttributeIteratorSignature = void(const AttributeInfo & attribute);
		using AttributeIteratorFunc = putils::function<AttributeIteratorSignature, KENGINE_ATTRIBUTE_ITERATOR_FUNC_SIZE>;

		struct ForEachAttribute : functions::BaseFunction<
			void(Entity & e, const AttributeIteratorFunc & func)
		> {};
	}
}

#define refltype kengine::meta::ForEachAttribute
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype