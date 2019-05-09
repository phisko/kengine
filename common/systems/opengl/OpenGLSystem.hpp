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
		// This format is not optimized, as the entity id could fit in with the normal or color
		// However, this has not caused any performance issues so far, and this being a learning project
		// for me I'd rather keep things clear and simple.
		// If this is a no-no for you, please open an issue on github and I'll change things around

		float position[4]; // x, y, z, depth
		float normal[4]; // x, y, z, ignore
		float color[4]; // r, g, b, ignore
		float entityID[4]; // id, ignore, ignore, ignore

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GBufferTextures::position),
			pmeta_reflectible_attribute(&GBufferTextures::normal),
			pmeta_reflectible_attribute(&GBufferTextures::color),
			pmeta_reflectible_attribute(&GBufferTextures::entityID)
		);
	};

	struct ModelLoaderComponent;

	class OpenGLSystem : public kengine::System<OpenGLSystem, kengine::packets::RegisterEntity,
		kengine::packets::DefineGBufferSize, kengine::packets::VertexDataAttributeIterator,
		kengine::packets::GetGBufferSize, kengine::packets::GetGBufferTexture> {
	public:
		OpenGLSystem(kengine::EntityManager & em);
		~OpenGLSystem();

		void execute() noexcept final;
		void onLoad(const char *) noexcept final;

		void handle(kengine::packets::RegisterEntity p);
		void handle(kengine::packets::DefineGBufferSize p);
		void handle(kengine::packets::VertexDataAttributeIterator p);
		void handle(kengine::packets::GetGBufferSize p);
		void handle(kengine::packets::GetGBufferTexture p);

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
