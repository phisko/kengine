#pragma once

#ifndef KENGINE_MODEL_LOADER_FUNCTION_SIZE
# define KENGINE_MODEL_LOADER_FUNCTION_SIZE 64
#endif

#include "Point.hpp"
#include "opengl/Program.hpp"
#include "function.hpp"

namespace detail {
	putils_nested_type_detector(DataType);
}

namespace kengine {
	struct ModelDataComponent {
		struct Mesh {
			struct DataInfo {
				size_t nbElements;
				size_t elementSize;
				const void * data;
			};

			DataInfo vertices;
			DataInfo indices;
			int indexType; // GLenum (GL_UNSIGNED_SHORT / GL_UNSIGNED_INT / ...)
		};

		std::vector<Mesh> meshes;

		~ModelDataComponent() {
			if (free != nullptr)
				free();
		}

		using FreeFunc = putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		FreeFunc free = nullptr;

		using RegisterVertexAttributesFunc = void();
		RegisterVertexAttributesFunc * registerVertexAttributes = nullptr;

		using VertexSizeFunc = size_t();
		VertexSizeFunc * getVertexSize = nullptr;

		using AttributeOffsetFunc = std::ptrdiff_t(const char * attributeName);
		AttributeOffsetFunc * getVertexAttributeOffset = nullptr;

		template<typename VertexType>
		void init() {
			static constexpr bool isPolyVoxType = ::detail::has_nested_type_DataType<VertexType>{};

			if constexpr (isPolyVoxType)
				registerVertexAttributes = putils::gl::setPolyvoxVertexType<VertexType>;
			else
				registerVertexAttributes = putils::gl::setVertexType<VertexType>;

			getVertexSize = [] { return sizeof(VertexType); };

			getVertexAttributeOffset = [](const char * attributeName) {
				const auto * vertex = (const VertexType *)nullptr;
				const void * attribute = (const char *)nullptr + sizeof(VertexType);

				if constexpr (isPolyVoxType) {
					if (strcmp("position", attributeName) == 0)
						attribute = &vertex->position;
					else if (strcmp("normal", attributeName) == 0)
						attribute = &vertex->normal;
					else
						putils::reflection::for_each_attribute<VertexType::DataType>([&](const auto name, const auto member) {
							if (strcmp(name, attributeName) != 0)
								return;
							attribute = &(vertex->data.*member);
						});
				}
				else
					putils::reflection::for_each_attribute<VertexType>([&](const auto name, const auto member) {
						if (strcmp(name, attributeName) != 0)
							return;
						attribute = &(vertex->*member);
					});

				std::ptrdiff_t ret = (const char *)attribute - (const char *)vertex;
				if (ret >= sizeof(VertexType))
					ret = -1;
				return ret;
			};
		}

		ModelDataComponent() = default;
		ModelDataComponent(ModelDataComponent &&) = default;
		ModelDataComponent & operator=(ModelDataComponent &&) = default;
	};
}