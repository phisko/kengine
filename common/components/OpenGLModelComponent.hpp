#pragma once

#include <vector>

#include <gl/glew.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

#include "Color.hpp"
#include "Entity.hpp"
#include "not_serializable.hpp"

namespace putils::gl { class Program; }

namespace kengine {
	// Used only by shaders, built from a ModelLoaderComponent
	struct OpenGLModelComponent : kengine::not_serializable {
		struct Mesh {
			GLuint vertexArrayObject = -1;
			GLuint vertexBuffer = -1;
			GLuint indexBuffer = -1;
			size_t nbIndices = 0;
			GLenum indexType = GL_UNSIGNED_INT;
		};

		std::vector<Mesh> meshes;

		void (*vertexRegisterFunc)() = nullptr;

		pmeta_get_class_name(OpenGLModelComponent);
	};
}