#include "highlight_shader.hpp"

// entt
#include <entt/entity/registry.hpp>

// kreogl
#include "kreogl/camera.hpp"
#include "kreogl/impl/raii/scoped_gl_feature.hpp"
#include "kreogl/impl/shaders/helpers/quad/quad_glsl.hpp"
#include "kreogl/impl/shapes/quad.hpp"

// kengine data
#include "kengine/data/highlight.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

kengine::highlight_shader::highlight_shader(const entt::registry & r) noexcept
	: _r(&r) {
	KENGINE_PROFILING_SCOPE;
	kengine_log(r, log, "kreogl", "Initializing highlight shader");

	init("highlight_shader");

	use_without_uniform_check();

	_glsl.gposition = (int)kreogl::gbuffer::texture::position;
	_glsl.guser_data = (int)kreogl::gbuffer::texture::user_data;
}

void kengine::highlight_shader::add_source_files() noexcept {
	KENGINE_PROFILING_SCOPE;
	kengine_log(*_r, verbose, "kreogl", "Adding source files for highlight shader");

	add_source_file(kreogl::quad_glsl::vert, GL_VERTEX_SHADER);
	add_source_file(highlight_glsl::frag, GL_FRAGMENT_SHADER);
}

std::vector<kreogl::uniform_base *> kengine::highlight_shader::get_uniforms() noexcept {
	kengine_log(*_r, verbose, "kreogl", "Getting uniforms for highlight shader");

	return {
		&_glsl.gposition,
		&_glsl.guser_data,
		&_glsl.view_pos,
		&_glsl.screen_size,
		&_glsl.entity_id,
		&_glsl.highlight_color,
		&_glsl.intensity
	};
}

void kengine::highlight_shader::draw(const kreogl::draw_params & params) noexcept {
	KENGINE_PROFILING_SCOPE;
	kengine_log(*_r, very_verbose, "kreogl", "Drawing highlight shader");

	auto uniform_checker = use(false);

	const kreogl::scoped_gl_feature blend(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_glsl.view_pos = params.camera.get_position();
	_glsl.screen_size = params.camera.get_viewport().get_resolution();

	for (const auto & [entity, highlight] : _r->view<data::highlight>().each()) {
		kengine_logf(*_r, very_verbose, "kreogl", "Drawing highlights for [%zu]", entity);

		uniform_checker.should_check = true;
		_glsl.entity_id = float(entity);
		_glsl.highlight_color = { highlight.color.r, highlight.color.g, highlight.color.b, highlight.color.a };
		_glsl.intensity = highlight.intensity * 2.f - 1.f; // convert from [0,1] to [-1,1]
		kreogl::shapes::draw_quad();
	}
}
