#pragma once

#include "packets/GBuffer.hpp"

#include "System.hpp"
#include "opengl/Program.hpp"
#include "GBuffer.hpp"

namespace kengine {

	// These are the textures used by the "default" shaders
	// If you want to use additional textures, you can either copy this struct or inherit from it,
	// so long as you are careful to declare the reflectible attributes in the same order
	// (as that order will define their GLSL locations)
	struct GBufferTextures {
		float position[4];
		float normal[4];
		float color[4];
		float entityID[4];

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GBufferTextures::position),
			pmeta_reflectible_attribute(&GBufferTextures::normal),
			pmeta_reflectible_attribute(&GBufferTextures::color),
			pmeta_reflectible_attribute(&GBufferTextures::entityID)
		);
	};

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
