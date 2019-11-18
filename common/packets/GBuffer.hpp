#pragma once

#include <functional>

namespace kengine::packets {
	struct DefineGBufferSize {
		size_t nbAttributes;
	};

	struct GBufferTexturesIterator {
		using AttributeFunc = const std::function<void(const char *)> &;
		std::function<void(AttributeFunc func)> func = nullptr;
	};
}

namespace kengine {
	template<typename Textures>
	void initGBuffer(kengine::EntityManager & em) {
		em.send(packets::GBufferTexturesIterator{
			[](auto func) {
				putils::reflection::for_each_attribute<Textures>([&](auto name, auto member) {
					func(name);
				});
			}
		});
		em.send(packets::DefineGBufferSize{ putils::reflection::get_attributes<Textures>().size });
	}
}