#pragma once

// kreogl
#include "kreogl/impl/shaders/Shader.hpp"
#include "kreogl/impl/shaders/Singleton.hpp"

// impl
#include "HighlightGLSL.hpp"

namespace kengine {
	class HighlightShader : public kreogl::Shader, public kreogl::Singleton<HighlightShader> {
	public:
		HighlightShader() noexcept;

	private:
		void draw(const kreogl::DrawParams & params) noexcept override;
		void addSourceFiles() noexcept override;
		std::vector<kreogl::UniformBase *> getUniforms() noexcept override;

	private:
		HighlightGLSL _glsl;
	};
}