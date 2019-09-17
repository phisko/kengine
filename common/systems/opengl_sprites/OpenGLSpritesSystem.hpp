#pragma once

#include "System.hpp"

namespace kengine {
	class EntityManager;

	class OpenGLSpritesSystem : public kengine::System<OpenGLSpritesSystem, kengine::packets::RegisterEntity> {
	public:
		OpenGLSpritesSystem(kengine::EntityManager & em);
		void onLoad(const char *) noexcept override;

		void handle(kengine::packets::RegisterEntity p);

	private:
		kengine::EntityManager & _em;
	};
}
