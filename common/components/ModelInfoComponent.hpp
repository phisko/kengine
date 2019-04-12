#pragma once

#include <functional>
#include <gl/glew.h>
#include <GL/GL.h>
#include <glm/glm.hpp>

namespace putils::gl { class Program; }

namespace kengine {
	// Used only by shaders, built from a ModelLoaderComponent
	struct ModelInfoComponent : kengine::not_serializable {
		struct Mesh {
			GLuint vertexArrayObject = -1;
			GLuint vertexBuffer = -1;
			GLuint indexBuffer = -1;
			size_t nbIndices = 0;
			GLenum indexType = GL_UNSIGNED_INT;
		};

		std::vector<Mesh> meshes;

		glm::vec3 translation = { 0.f, 0.f, 0.f };
		float pitch = 0.f;
		float yaw = 0.f;

		std::function<void()> vertexRegisterFunc = nullptr;

		pmeta_get_class_name(ModelInfoComponent);
	};

	struct ModelInfoTexturesComponent : kengine::not_serializable {
		struct MeshTextures {
			std::vector<unsigned int> diffuse;
			std::vector<unsigned int> specular;
		};

		std::vector<MeshTextures> meshes; // Must match those in ModelInfoComponent

		pmeta_get_class_name(ModelInfoTexturesComponent);
	};
}