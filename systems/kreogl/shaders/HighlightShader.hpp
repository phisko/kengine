#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "default_constructors.hpp"

// kreogl
#include "kreogl/impl/shaders/Shader.hpp"
#include "kreogl/impl/shaders/Singleton.hpp"

// impl
#include "HighlightGLSL.hpp"

namespace kengine {
	class HighlightShader : public kreogl::Shader {
	public:
		PUTILS_MOVE_ONLY(HighlightShader);

		HighlightShader(const entt::registry & r) noexcept;

	private:
		void draw(const kreogl::DrawParams & params) noexcept override;
		void addSourceFiles() noexcept override;
		std::vector<kreogl::UniformBase *> getUniforms() noexcept override;

	private:
		HighlightGLSL _glsl;
		const entt::registry * _r = nullptr;
	};
}