#pragma once

#ifndef KENGINE_MODEL_LOADER_FUNCTION_SIZE
# define KENGINE_MODEL_LOADER_FUNCTION_SIZE 64
#endif

#include "Point.hpp"
#include "opengl/Program.hpp"
#include "function.hpp"

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

		using FreeFunc = putils::function<void(), KENGINE_MODEL_LOADER_FUNCTION_SIZE>;
		FreeFunc free;

		using VertexRegisterFunc = void(*)();
		VertexRegisterFunc vertexRegisterFunc;
	};
}