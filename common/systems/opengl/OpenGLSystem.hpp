#pragma once

#include "packets/EntityInPixel.hpp"
#include "packets/GBuffer.hpp"
#include "packets/CaptureMouse.hpp"

#include "System.hpp"

namespace putils::gl { class Program; }

namespace kengine {

	// These are the textures used by the "default" shaders
	// If you want to use additional textures, you can either copy this struct or inherit from it,
	// so long as you are careful to declare the reflectible attributes in the same order
	// (as that order will define their GLSL locations)
	struct GBufferTextures {
		// This format is not optimized, as the entity id could fit in with the normal
		// However, this has not caused any performance issues so far, and this being a learning project
		// for me I'd rather keep things clear and simple.
		// If this is a no-no for you, please open an issue on github and I'll change things around

		float position[4]; // x, y, z, depth
		float normal[4]; // x, y, z, ignore
		float color[4]; // r, g, b, shouldIgnoreLighting
		float entityID[4]; // id, ignore, ignore, ignore

		putils_reflection_attributes(
			putils_reflection_attribute(&GBufferTextures::position),
			putils_reflection_attribute(&GBufferTextures::normal),
			putils_reflection_attribute(&GBufferTextures::color),
			putils_reflection_attribute(&GBufferTextures::entityID)
		);
	};

	struct ModelDataComponent;

	class OpenGLSystem : public System<OpenGLSystem,
		packets::RegisterEntity, packets::RemoveEntity,
		packets::DefineGBufferSize, packets::GBufferTexturesIterator,
		packets::GetEntityInPixel, packets::CaptureMouse
	> {
	public:
		OpenGLSystem(EntityManager & em);
		~OpenGLSystem();

		void execute() noexcept final;

		void handle(packets::RegisterEntity p);
		void handle(packets::RemoveEntity p);
		void handle(packets::DefineGBufferSize p);
		void handle(packets::GBufferTexturesIterator p);
		void handle(packets::GetEntityInPixel p);
		void handle(packets::CaptureMouse p);

	private:
		void init() noexcept;
		void addShaders() noexcept;

		void doOpenGL() noexcept;
		void doImGui() noexcept;
		void updateWindowProperties() noexcept;

		void initShader(putils::gl::Program & p);

	private:
		EntityManager & _em;

	private:
		packets::GBufferTexturesIterator _gBufferIterator;
	};
}
