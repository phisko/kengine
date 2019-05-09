#pragma once

namespace kengine::packets {
	struct GetGBufferSize {
		putils::Point2ui & size;
	};

	struct GetGBufferTexture { // WARNING: texture is flipped vertically (you'll want to do `y = (height - y)` before indexing)
		size_t textureIndex;
		float * buff;
		size_t buffSize;
	};

	struct DefineGBufferSize {
		size_t nbAttributes;
	};

	struct VertexDataAttributeIterator {
		using AttributeFunc = const std::function<void(const char *)> &;
		std::function<void(AttributeFunc func)> func = nullptr;
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
		em.send(packets::DefineGBufferSize{ Textures::get_attributes().size });
	}
}