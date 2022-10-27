#pragma once

#ifndef KENGINE_MODEL_LOADER_FUNCTION_SIZE
# define KENGINE_MODEL_LOADER_FUNCTION_SIZE 64
#endif

#include "Point.hpp"
#include "function.hpp"
#include "reflection.hpp"
#include "meta/traits/has_nested_type.hpp"

namespace detail {
	putils_nested_type_detector(DataType);
}

namespace kengine {
	struct ModelDataComponent {
		struct Mesh {
			struct Buffer {
				size_t nbElements;
				size_t elementSize;
				const void * data;
			};

			Buffer vertices;
			Buffer indices;
			putils::meta::type_index indexType; // unsigned int, unsigned short...
		};

		std::vector<Mesh> meshes;

		~ModelDataComponent() {
			if (free != nullptr)
				free();
		}

		using FreeFunc = putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		FreeFunc free = nullptr;

		struct VertexAttribute {
			const char * name;
			size_t offset;
			putils::meta::type_index type;
		};

		std::vector<VertexAttribute> vertexAttributes;
		size_t vertexSize;

		template<typename VertexType>
		void init() noexcept {
			static constexpr bool isPolyVoxType = ::detail::has_nested_type_DataType<VertexType>{};

			if constexpr (isPolyVoxType) {
				vertexAttributes.push_back({ "position", offsetof(VertexType, position), putils::meta::type<float[3]>::index });

				using Data = typename VertexType::DataType;
				const auto dataOffset = offsetof(VertexType, data);
				putils::reflection::for_each_attribute<Data>([&](const auto & attr) noexcept {
					using Member = putils::MemberType<putils_typeof(attr.ptr)>;
					vertexAttributes.push_back({ attr.name, dataOffset + (size_t)putils::member_offset(attr.ptr), putils::meta::type<Member>::index });
				});
			}
			else {
				putils::reflection::for_each_attribute<VertexType>([&](const auto & attr) noexcept {
					using Member = putils::MemberType<putils_typeof(attr.ptr)>;
					vertexAttributes.push_back({ attr.name, (size_t)putils::member_offset(attr.ptr), putils::meta::type<Member>::index });
				});
			}

			vertexSize = sizeof(VertexType);
		}

		ModelDataComponent() = default;
		ModelDataComponent(ModelDataComponent &&) = default;
		ModelDataComponent & operator=(ModelDataComponent &&) = default;
	};
}