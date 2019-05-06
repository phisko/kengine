#pragma once

namespace kengine::packets {
	struct GBufferSize {
		size_t nbAttributes;
	};

	struct VertexDataAttributeIterator {
		using AttributeFunc = const std::function<void(const char *)> &;
		std::function<void(AttributeFunc func)> func = nullptr;
	};

	struct GetGBufferTexture {
		size_t textureIndex;
		float * buff;
		size_t buffSize;
	};
}

namespace kengine {
	template<typename Textures>
	void initGBuffer(kengine::EntityManager & em) {
		em.send(packets::VertexDataAttributeIterator{
			[](auto func) {
				putils::for_each_attribute(Textures::get_attributes(), [&](auto name, auto member) {
					func(name);
				});
			}
		});
		em.send(packets::GBufferSize{ Textures::get_attributes().size });
	}
}