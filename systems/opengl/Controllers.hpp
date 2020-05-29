#pragma once

#include "EntityCreator.hpp"
#include "functions/InitGBuffer.hpp"

namespace kengine::opengl {
	struct ShaderProfileComponent {
		float executionTime;
	};

	EntityCreatorFunctor<64> ShaderController(EntityManager & em);
	EntityCreatorFunctor<64> GBufferDebugger(EntityManager & em, const functions::GBufferAttributeIterator & iterator);
}
