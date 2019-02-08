#pragma once

namespace kengine::packets {
	struct GBufferSize {
		size_t nbAttributes;
	};

	struct VertexDataAttributeIterator {
		using AttributeFunc = const std::function<void(const char *)> &;
		std::function<void(AttributeFunc func)> func;
	};
}

namespace kengine {
	template<typename VertexData>
	void initGBufferWithVertexType(kengine::ISystem & s) {
		s.send(packets::GBufferSize{ VertexData::get_attributes().size });
		s.send(packets::VertexDataAttributeIterator{
			[](auto func) {
				putils::for_each_attribute(VertexData::get_attributes(), [&](auto name, auto member) {
					func(name);
				});
			}
		});
	}
}