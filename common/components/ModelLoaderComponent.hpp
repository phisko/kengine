#pragma once

#include "Point.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	struct ModelLoaderComponent {
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

		std::function<ModelData()> func;
		std::function<void(putils::gl::Program & p)> vertexRegisterFunc;

		pmeta_get_class_name(ModelLoaderComponent);
	};
}