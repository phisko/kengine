#pragma once

#include <PolyVox/RawVolume.h>

#include "packets/GBuffer.hpp"

#include "System.hpp"
#include "opengl/Program.hpp"
#include "GBuffer.hpp"

namespace kengine {
	struct MeshLoaderComponent;

	struct MeshInfoComponent {
		GLuint vertexArrayObject = -1;
		GLuint vertexBuffer = -1;
		GLuint indexBuffer = -1;
		size_t nbIndices = 0;
		glm::vec3 translation = { 0.f, 0.f, 0.f };
		GLenum indexType = GL_UNSIGNED_INT;

		std::function<void(putils::gl::Program & p)> vertexRegisterFunc = nullptr;
	};

	class OpenGLSystem : public kengine::System<OpenGLSystem, kengine::packets::RegisterEntity, kengine::packets::GBufferSize, kengine::packets::VertexDataAttributeIterator> {
	public:
		OpenGLSystem(kengine::EntityManager & em);
		~OpenGLSystem();

		void execute() noexcept final;
		void onLoad() noexcept final;

		void handle(kengine::packets::RegisterEntity p);
		void handle(kengine::packets::GBufferSize p);
		void handle(kengine::packets::VertexDataAttributeIterator p);

	private:
		void createObject(kengine::Entity & e, const kengine::MeshLoaderComponent & meshLoader);

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
		GBuffer _gBuffer;
		kengine::packets::VertexDataAttributeIterator _gBufferIterator;
	};
}
