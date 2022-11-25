#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/default_constructors.hpp"

// kreogl
#include "kreogl/impl/shaders/shader.hpp"
#include "kreogl/impl/shaders/singleton.hpp"

// impl
#include "highlight_glsl.hpp"

namespace kengine {
	class highlight_shader : public kreogl::shader {
	public:
		PUTILS_MOVE_ONLY(highlight_shader);

		highlight_shader(const entt::registry & r) noexcept;

	private:
		void draw(const kreogl::draw_params & params) noexcept override;
		void add_source_files() noexcept override;
		std::vector<kreogl::uniform_base *> get_uniforms() noexcept override;

	private:
		highlight_glsl _glsl;
		const entt::registry * _r = nullptr;
	};
}