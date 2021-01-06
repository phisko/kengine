#pragma once

#include <utility>
#include "BaseFunction.hpp"
#include "kengine.hpp"

#ifndef KENGINE_GBUFFER_ATTRIBUTE_ITERATOR_MAX_SIZE
# define KENGINE_GBUFFER_ATTRIBUTE_ITERATOR_MAX_SIZE 64
#endif

namespace kengine::functions {
	using GBufferAttributeFunc = const putils::function<void(const char *), KENGINE_GBUFFER_ATTRIBUTE_ITERATOR_MAX_SIZE> &;
	using GBufferAttributeIterator = void (*)(GBufferAttributeFunc iterator);

	struct InitGBuffer : BaseFunction<
		void(size_t nbAttributes, const GBufferAttributeIterator & iterator)
	> {};
}

namespace kengine {
	template<typename Textures>
	void initGBuffer() {
		for (const auto & [e, initGBuffer] : entities.with<functions::InitGBuffer>()) {
			initGBuffer(std::tuple_size_v<putils_typeof(putils::reflection::get_attributes<Textures>())>,
				[](auto func) {
					putils::reflection::for_each_attribute<Textures>([&](auto name, auto member) {
						func(name);
					});
				}
			);
		}
	}
}

#define refltype kengine::functions::InitGBuffer
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype
