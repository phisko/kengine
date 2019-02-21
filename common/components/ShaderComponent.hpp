#pragma once

#include <memory>

namespace putils::gl { class Program; }

namespace kengine {
	struct ShaderComponent {
		std::unique_ptr<putils::gl::Program> shader;
		bool enabled = true;
	};

	struct GBufferShaderComponent : ShaderComponent {};
	struct LightingShaderComponent : ShaderComponent {};
	struct PostProcessShaderComponent : ShaderComponent {};

	template<typename T, typename ...Args>
	static inline GBufferShaderComponent makeGBufferShaderComponent(Args && ... args) {
		return GBufferShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	template<typename T, typename ...Args>
	static inline LightingShaderComponent makeLightingShaderComponent(Args && ... args) {
		return LightingShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}

	template<typename T, typename ...Args>
	static inline PostProcessShaderComponent makePostProcessShaderComponent(Args && ... args) {
		return PostProcessShaderComponent{ std::make_unique<T>(FWD(args)...) };
	}
}