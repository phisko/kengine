#pragma once

#include <vector>
#include "opengl/RAII.hpp"

namespace putils::gl { class Program; }

namespace kengine {
	// Used only by shaders, built from a ModelDataComponent
	struct OpenGLModelComponent {
		struct Mesh {
			putils::gl::VertexArray vertexArrayObject;
			putils::gl::Buffer vertexBuffer;
			putils::gl::Buffer indexBuffer;
			size_t nbIndices = 0;
			GLenum indexType = GL_UNSIGNED_INT;
		};

		std::vector<Mesh> meshes;

		void (*vertexRegisterFunc)() = nullptr;
	};
}