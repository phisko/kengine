#pragma once

#include <memory>

namespace kengine {
	template<typename T>
	struct SystemSpecificShaderComponent {
		std::unique_ptr<T> shader;
		bool enabled = true;
	};

	struct GBufferShaderComponent {};
	struct LightingShaderComponent {};
	struct PostLightingShaderComponent {};
	struct PostProcessShaderComponent {};

	struct ShadowMapShaderComponent {};
	struct ShadowCubeShaderComponent {};
}
