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
		void init() {
			static constexpr bool isPolyVoxType = ::detail::has_nested_type_DataType<VertexType>{};

			if constexpr (isPolyVoxType) {
				vertexAttributes.push_back({ "position", offsetof(VertexType, position), putils::meta::type<float[3]>::index });
				vertexAttributes.push_back({ "normal", offsetof(VertexType, normal), putils::meta::type<float[3]>::index });

				using Data = typename VertexType::DataType;
				const auto dataOffset = offsetof(VertexType, data);
				putils::reflection::for_each_attribute<Data>([&](const char * name, auto member) {
					using Member = putils::MemberType<putils_typeof(member)>;
					vertexAttributes.push_back({ name, dataOffset + (size_t)putils::member_offset(member), putils::meta::type<Member>::index });
				});
			}
			else {
				putils::reflection::for_each_attribute<VertexType>([&](const char * name, auto member) {
					using Member = putils::MemberType<putils_typeof(member)>;
					vertexAttributes.push_back({ name, (size_t)putils::member_offset(member), putils::meta::type<Member>::index });
				});
			}

			vertexSize = sizeof(VertexType);
		}

		ModelDataComponent() = default;
		ModelDataComponent(ModelDataComponent &&) = default;
		ModelDataComponent & operator=(ModelDataComponent &&) = default;
	};
}