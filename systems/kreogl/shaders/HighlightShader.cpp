#include "HighlightShader.hpp"

// kengine
#include "kengine.hpp"
#include "data/HighlightComponent.hpp"

// kreogl
#include "kreogl/Camera.hpp"
#include "kreogl/impl/RAII/ScopedGLFeature.hpp"
#include "kreogl/impl/shaders/helpers/Quad/QuadGLSL.hpp"
#include "kreogl/impl/shapes/Quad.hpp"

kengine::HighlightShader::HighlightShader() noexcept {
	init("HighlightShader");

	useWithoutUniformCheck();

	_glsl.gposition = (int)kreogl::GBuffer::Texture::Position;
	_glsl.gnormal = (int)kreogl::GBuffer::Texture::Normal;
	_glsl.guserData = (int)kreogl::GBuffer::Texture::UserData;
}

void kengine::HighlightShader::addSourceFiles() noexcept {
	addSourceFile(kreogl::QuadGLSL::vert, GL_VERTEX_SHADER);
	addSourceFile(HighlightGLSL::frag, GL_FRAGMENT_SHADER);
}

std::vector<kreogl::UniformBase *> kengine::HighlightShader::getUniforms() noexcept {
	return {
		&_glsl.gposition,
		&_glsl.gnormal,
		&_glsl.guserData,
		&_glsl.viewPos,
		&_glsl.screenSize,
		&_glsl.entityID,
		&_glsl.highlightColor,
		&_glsl.intensity
	};
}

void kengine::HighlightShader::draw(const kreogl::DrawParams & params) noexcept {
	auto uniformChecker = use(false);

	const kreogl::ScopedGLFeature blend(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_glsl.viewPos = params.camera.getPosition();
	_glsl.screenSize = params.camera.getViewport().getResolution();

	for (const auto & [entity, highlight] : entities.with<HighlightComponent>()) {
		uniformChecker.shouldCheck = true;
		_glsl.entityID = float(entity.id);
		_glsl.highlightColor = { highlight.color.r, highlight.color.g, highlight.color.b, highlight.color.a };
		_glsl.intensity = highlight.intensity * 2.f - 1.f; // convert from [0,1] to [-1,1]
		kreogl::shapes::drawQuad();
	}
}
