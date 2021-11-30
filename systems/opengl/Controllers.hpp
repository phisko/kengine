#pragma once

#include "EntityCreator.hpp"
#include "functions/InitGBuffer.hpp"

namespace kengine::opengl {
	struct ShaderProfileComponent {
		float executionTime;
	};

	EntityCreator * ShaderController() noexcept;
	EntityCreatorFunctor<64> GBufferDebugger(const functions::GBufferAttributeIterator & iterator) noexcept;
}
