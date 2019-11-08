#pragma once

#include "packets/EntityInPixel.hpp"
#include "packets/GBuffer.hpp"
#include "packets/CaptureMouse.hpp"

#include "System.hpp"
#include "GBuffer.hpp"

namespace putils::gl { class Program; }

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
		float color[4]; // r, g, b, shouldIgnoreLighting
		float entityID[4]; // id, ignore, ignore, ignore

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&GBufferTextures::position),
			pmeta_reflectible_attribute(&GBufferTextures::normal),
			pmeta_reflectible_attribute(&GBufferTextures::color),
			pmeta_reflectible_attribute(&GBufferTextures::entityID)
		);
	};

	struct ModelLoaderComponent;

	class OpenGLSystem : public System<OpenGLSystem, packets::RegisterEntity,
		packets::DefineGBufferSize, packets::GBufferTexturesIterator,
		packets::GetGBufferSize, packets::GetGBufferTexture,
		packets::GetEntityInPixel, packets::CaptureMouse
	> {
	public:
		OpenGLSystem(EntityManager & em);
		~OpenGLSystem();

		void execute() noexcept final;
		void onLoad(const char *) noexcept final;

		void handle(packets::RegisterEntity p);
		void handle(packets::DefineGBufferSize p);
		void handle(packets::GBufferTexturesIterator p);
		void handle(packets::GetGBufferSize p);
		void handle(packets::GetGBufferTexture p);
		void handle(packets::GetEntityInPixel p);
		void handle(packets::CaptureMouse p);

	private:
		void createObject(Entity & e, const ModelLoaderComponent & meshLoader);

		void init() noexcept;
		void handleInput() noexcept;
		void addShaders() noexcept;
		void doOpenGL() noexcept;

		void initShader(putils::gl::Program & p);

		void debugTexture(GLint texture);

	private:
		EntityManager & _em;

	private:
		GBuffer _gBuffer;
		packets::GBufferTexturesIterator _gBufferIterator;
	};
}
