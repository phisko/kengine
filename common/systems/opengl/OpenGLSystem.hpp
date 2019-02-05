#pragma once

#include <PolyVox/RawVolume.h>

#include "System.hpp"
#include "opengl/Program.hpp"
#include "GBuffer.hpp"

namespace kengine {
	struct VertexData {
		float color[3] = { 0.f, 0.f, 0.f };

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&VertexData::color)
		);

		bool operator==(size_t i) const {
			return color[0] == 0.f && color[1] == 0.f && color[2] == 0.f;
		}

		bool operator>(size_t i) const {
			return color[0] != 0.f || color[1] != 0.f || color[2] != 0.f;
		}

		bool operator==(const VertexData & rhs) const {
			return color[0] == rhs.color[0] && color[1] == rhs.color[1] && color[2] == rhs.color[2];
		}
	};

	// Mesh

	struct MeshComponent {
		kengine::Entity::ID meshInfo = kengine::Entity::INVALID_ID;
	};

	struct MeshInfoComponent {
		GLuint vertexArrayObject = -1;
		GLuint vertexBuffer = -1;
		GLuint indexBuffer = -1;
		size_t nbIndices = 0;
		glm::vec3 translation = { 0.f, 0.f, 0.f };
		GLenum indexType = GL_UNSIGNED_INT;

		std::function<void(putils::gl::Program & p)> vertexRegisterFunc = nullptr;

		~MeshInfoComponent() {
			if (vertexBuffer != -1)
				glDeleteBuffers(1, &vertexBuffer);
			if (indexBuffer != -1)
				glDeleteBuffers(1, &indexBuffer);
			if (vertexArrayObject != -1)
				glDeleteVertexArrays(1, &vertexArrayObject);
		}

		float zOffset = 0.f;
	};

	class OpenGLSystem : public kengine::System<OpenGLSystem, kengine::packets::RegisterEntity> {
	public:
		OpenGLSystem(kengine::EntityManager & em);
		~OpenGLSystem();

		void execute() noexcept final;
		void onLoad() noexcept final;

		void handle(kengine::packets::RegisterEntity p);

	private:
		void registerMeshInfos() noexcept;
		kengine::Entity::ID createObject(PolyVox::RawVolume<VertexData> & volData);

		void handleInput() noexcept;
		void addShaders() noexcept;
		void initOpenGL() noexcept;
		void doOpenGL() noexcept;

		void initShader(putils::gl::Program & p);

		void drawObjects(GLint modelMatrixLocation) const noexcept;

		void debugTexture(GLint texture);

	private:
		kengine::EntityManager & _em;

	private:
		GBuffer<VertexData> _gBuffer;

	private:
		kengine::Entity::ID _sphereMeshInfo = kengine::Entity::INVALID_ID;
		kengine::Entity::ID _lightMeshInfo = kengine::Entity::INVALID_ID;

	private:
	};
}
