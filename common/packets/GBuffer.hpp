#pragma once

namespace kengine::packets {
	struct GBufferSize {
		size_t nbAttributes;
	};

	struct VertexDataAttributeIterator {
		using AttributeFunc = const std::function<void(const char *)> &;
		std::function<void(AttributeFunc func)> func = nullptr;
	};
}

namespace kengine {
	template<typename Textures>
	void initGBuffer(kengine::ISystem & s) {
		s.send(packets::VertexDataAttributeIterator{
			[](auto func) {
				putils::for_each_attribute(Textures::get_attributes(), [&](auto name, auto member) {
					func(name);
				});
			}
		});
		s.send(packets::GBufferSize{ Textures::get_attributes().size });
	}
}