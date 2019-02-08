#pragma once

#include "Point.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	struct MeshLoaderComponent {
		struct MeshData {
			struct DataInfo {
				size_t nbElements;
				size_t elementSize;
				const void * data;
			};

			DataInfo vertices;
			DataInfo indices;
			int indexType; // GLenum (GL_UNSIGNED_SHORT / GL_UNSIGNED_INT / ...)
			putils::Vector3f offsetToCentre;
		};

		std::function<MeshData()> func;
		std::function<void(putils::gl::Program & p)> vertexRegisterFunc;
	};
}