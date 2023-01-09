#pragma once

#ifndef KENGINE_MODEL_LOADER_FUNCTION_SIZE
#define KENGINE_MODEL_LOADER_FUNCTION_SIZE 64
#endif

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/function.hpp"
#include "putils/default_constructors.hpp"

namespace kengine::data {
	struct model_data {
		PUTILS_MOVE_ONLY(model_data);

		struct mesh {
			struct buffer {
				size_t nb_elements;
				size_t element_size;
				const void * data;
			};

			buffer vertices;
			buffer indices;
			putils::meta::type_index index_type; // unsigned int, unsigned short...
		};

		std::vector<mesh> meshes;

		~model_data() {
			if (free != nullptr)
				free();
		}

		using free_func = putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		free_func free = nullptr;

		struct vertex_attribute {
			const char * name;
			size_t offset;
			putils::meta::type_index type;
		};

		std::vector<vertex_attribute> vertex_attributes;
		size_t vertex_size;

		template<typename VertexType>
		void init() noexcept {
			static constexpr bool is_polyvox_type = requires { typename VertexType::DataType; };

			if constexpr (is_polyvox_type) {
				vertex_attributes.push_back({ "position", offsetof(VertexType, position), putils::meta::type<float[3]>::index });

				using Data = typename VertexType::DataType;
				const auto dataOffset = offsetof(VertexType, data);
				putils::reflection::for_each_attribute<Data>([&](const auto & attr) noexcept {
					using member = putils::member_type<putils_typeof(attr.ptr)>;
					vertex_attributes.push_back({ attr.name, dataOffset + (size_t)putils::member_offset(attr.ptr), putils::meta::type<member>::index });
				});
			}
			else {
				putils::reflection::for_each_attribute<VertexType>([&](const auto & attr) noexcept {
					using member = putils::member_type<putils_typeof(attr.ptr)>;
					vertex_attributes.push_back({ attr.name, (size_t)putils::member_offset(attr.ptr), putils::meta::type<member>::index });
				});
			}

			vertex_size = sizeof(VertexType);
		}
	};
}

#define refltype kengine::data::model_data
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(meshes),
		putils_reflection_attribute(free),
		putils_reflection_attribute(vertex_attributes),
		putils_reflection_attribute(vertex_size)
	);
};
#undef refltype

#define refltype kengine::data::model_data::mesh
putils_reflection_info {
	putils_reflection_custom_class_name(model_data_mesh);
	putils_reflection_attributes(
		putils_reflection_attribute(vertices),
		putils_reflection_attribute(indices),
		putils_reflection_attribute(index_type)
	);
};
#undef refltype

#define refltype kengine::data::model_data::mesh::buffer
putils_reflection_info {
	putils_reflection_custom_class_name(model_data_mesh_buffer);
	putils_reflection_attributes(
		putils_reflection_attribute(nb_elements),
		putils_reflection_attribute(element_size),
		putils_reflection_attribute(data)
	);
};
#undef refltype

#define refltype kengine::data::model_data::vertex_attribute
putils_reflection_info {
	putils_reflection_custom_class_name(model_data_vertex_attribute);
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(offset),
		putils_reflection_attribute(type)
	);
};
#undef refltype
