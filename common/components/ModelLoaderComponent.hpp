#pragma once

#ifndef KENGINE_MODEL_LOADER_FUNCTION_SIZE
# define KENGINE_MODEL_LOADER_FUNCTION_SIZE 64
#endif

#include "Point.hpp"
#include "opengl/Program.hpp"
#include "function.hpp"

namespace kengine {
	struct ModelLoaderComponent : kengine::not_serializable {

		struct ModelData {
			struct MeshData {
				struct DataInfo {
					size_t nbElements;
					size_t elementSize;
					const void * data;
				};

				DataInfo vertices;
				DataInfo indices;
				int indexType; // GLenum (GL_UNSIGNED_SHORT / GL_UNSIGNED_INT / ...)
			};

			std::vector<MeshData> meshes;
			putils::Vector3f offsetToCentre;
			putils::Vector3f scale = { 1.f, 1.f, 1.f };
			float pitch = 0.f;
			float yaw = 0.f;
		};

		using function = putils::function<ModelData(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		function func;
		using VertexRegisterFunc = void(*)();
		VertexRegisterFunc vertexRegisterFunc;

		pmeta_get_class_name(ModelLoaderComponent);

		template<typename Func>
		ModelLoaderComponent(Func && func, VertexRegisterFunc vertexRegisterFunc)
			: func(FWD(func)), vertexRegisterFunc(vertexRegisterFunc)
		{}

		ModelLoaderComponent() = default;
		ModelLoaderComponent(const ModelLoaderComponent &) = default;
		ModelLoaderComponent & operator=(const ModelLoaderComponent &) = default;
		ModelLoaderComponent(ModelLoaderComponent &&) = default;
		ModelLoaderComponent & operator=(ModelLoaderComponent &&) = default;
	};
}