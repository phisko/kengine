#pragma once

#ifndef KENGINE_MODEL_LOADER_FUNCTION_SIZE
#define KENGINE_MODEL_LOADER_FUNCTION_SIZE 64
#endif

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/function.hpp"
#include "putils/default_constructors.hpp"

namespace kengine::render {
	//! putils reflect all
	//! used_types: [refltype::mesh, refltype::vertex_attribute]
	struct model_data {
		PUTILS_MOVE_ONLY(model_data);

		//! putils reflect all
		//! class_name: model_data_mesh
		//! used_types: [refltype::buffer]
		struct mesh {
			//! putils reflect all
			//! class_name: model_data_mesh_buffer
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

		//! putils reflect all
		//! class_name: model_data_vertex_attribute
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

#include "model_data.rpp"