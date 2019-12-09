#pragma once

#include "BaseFunction.hpp"
#include "EntityManager.hpp"

#ifndef KENGINE_GBUFFER_ATTRIBUTE_ITERATOR_MAX_SIZE
# define KENGINE_GBUFFER_ATTRIBUTE_ITERATOR_MAX_SIZE 64
#endif

namespace kengine::functions {
	using GBufferAttributeFunc = const putils::function<void(const char *), KENGINE_GBUFFER_ATTRIBUTE_ITERATOR_MAX_SIZE> &;
	using GBufferAttributeIterator = void (*)(GBufferAttributeFunc iterator);

	struct InitGBuffer : BaseFunction<
		void(size_t nbAttributes, const GBufferAttributeIterator & iterator)
	> {
		putils_reflection_class_name(InitGBuffer);
	};
}

namespace kengine {
	template<typename Textures>
	void initGBuffer(kengine::EntityManager & em) {
		for (const auto & [e, func] : em.getEntities<functions::InitGBuffer>()) {
			func(putils::reflection::get_attributes<Textures>().size,
				[](auto func) {
					putils::reflection::for_each_attribute<Textures>([&](auto name, auto member) {
						func(name);
					});
				}
			);
		}
	}
}
