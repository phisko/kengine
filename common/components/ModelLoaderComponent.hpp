#pragma once

#include "Point.hpp"
#include "opengl/Program.hpp"

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
			float pitch = 0.f;
			float yaw = 0.f;
		};

		ModelLoaderComponent(const std::function<ModelData()> & func, const std::function<void(putils::gl::Program & p)> & vertexRegisterFunc)
			: func(func), vertexRegisterFunc(vertexRegisterFunc)
		{}

		ModelLoaderComponent() = default;
		ModelLoaderComponent(const ModelLoaderComponent &) = default;
		ModelLoaderComponent & operator=(const ModelLoaderComponent &) = default;
		ModelLoaderComponent(ModelLoaderComponent &&) = default;
		ModelLoaderComponent & operator=(ModelLoaderComponent &&) = default;

		std::function<ModelData()> func;
		std::function<void(putils::gl::Program & p)> vertexRegisterFunc;

		pmeta_get_class_name(ModelLoaderComponent);
	};
}