#pragma once

#include <PolyVox/RawVolume.h>

#include "packets/GBuffer.hpp"

#include "System.hpp"
#include "opengl/Program.hpp"
#include "GBuffer.hpp"

namespace kengine {
	struct ModelLoaderComponent;

	class OpenGLSystem : public kengine::System<OpenGLSystem, kengine::packets::RegisterEntity, kengine::packets::GBufferSize, kengine::packets::VertexDataAttributeIterator> {
	public:
		OpenGLSystem(kengine::EntityManager & em);
		~OpenGLSystem();

		void execute() noexcept final;
		void onLoad(const char *) noexcept final;

		void handle(kengine::packets::RegisterEntity p);
		void handle(kengine::packets::GBufferSize p);
		void handle(kengine::packets::VertexDataAttributeIterator p);

	private:
		void createObject(kengine::Entity & e, const kengine::ModelLoaderComponent & meshLoader);

		void handleInput() noexcept;
		void addShaders() noexcept;
		void initOpenGL() noexcept;
		void doOpenGL() noexcept;

		void initShader(putils::gl::Program & p);

		void debugTexture(GLint texture);

	private:
		kengine::EntityManager & _em;

	private:
		GBuffer _gBuffer;
		kengine::packets::VertexDataAttributeIterator _gBufferIterator;
	};
}
