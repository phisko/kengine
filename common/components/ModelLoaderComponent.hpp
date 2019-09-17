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
		};

		using LoadingFunc = putils::function<ModelData(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		LoadingFunc load;

		using FreeFunc = putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		FreeFunc free;

		using VertexRegisterFunc = void(*)();
		VertexRegisterFunc vertexRegisterFunc;

		pmeta_get_class_name(ModelLoaderComponent);

		template<typename Loader, typename Releaser>
		ModelLoaderComponent(Loader && loader, Releaser && releaser, VertexRegisterFunc vertexRegisterFunc)
			: load(FWD(loader)), free(FWD(releaser)), vertexRegisterFunc(vertexRegisterFunc)
		{}

		ModelLoaderComponent() = default;
		ModelLoaderComponent(const ModelLoaderComponent &) = default;
		ModelLoaderComponent & operator=(const ModelLoaderComponent &) = default;
		ModelLoaderComponent(ModelLoaderComponent &&) = default;
		ModelLoaderComponent & operator=(ModelLoaderComponent &&) = default;
	};
}