#include <random>
#include "SSAO.hpp"
#include "string.hpp"
#include "EntityManager.hpp"
#include "shaders/shaders.hpp"

#include "components/AdjustableComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

static void initTexture(GLuint & fbo, GLuint & texture, size_t width, size_t height) {
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei)width, (GLsizei)height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

static auto RADIUS_VALUE = .01f;
static auto FARCLIP_VALUE = 0.1f;
static auto BIAS_VALUE = .3f;
namespace kengine::Shaders {
	SSAO::SSAO(kengine::EntityManager & em)
		: putils::gl::Program(true, pmeta_nameof(SSAO))
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/SSAO] Radius", &RADIUS_VALUE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/SSAO] Farclip", &FARCLIP_VALUE); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/SSAO] Bias", &BIAS_VALUE); };
	}


	void SSAO::init(::size_t firstTextureID, ::size_t screenWidth, ::size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SSAO>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::SSAO::frag, GL_FRAGMENT_SHADER }
		));

		initTexture(_fbo, _colorBuffer, screenWidth, screenHeight);
		texturesToDebug.push_back(Texture{ "SSAO", _colorBuffer });

		const std::uniform_real_distribution<float> randomFloats(0.f, 1.f);
		std::default_random_engine generator(std::random_device{}());

		for (size_t i = 0; i < 64; ++i) {
			const glm::vec3 sample(randomFloats(generator) * 2.f - 1.f, randomFloats(generator) * 2.f - 1.f, randomFloats(generator));
			putils::gl::setUniform(samples[i], glm::normalize(sample));
		}
	}

	void SSAO::run(const glm::mat4 & view, const glm::mat4 & projection, size_t screenWidth, size_t screenHeight) {
		ShaderHelper::BindFramebuffer __f(_fbo);
		glClear(GL_COLOR_BUFFER_BIT);

		use();
		putils::gl::setUniform(this->view, view);
		putils::gl::setUniform(this->proj, projection);
		putils::gl::setUniform(this->screenSize, glm::vec2(screenWidth, screenHeight));

		putils::gl::setUniform(RADIUS, RADIUS_VALUE);
		putils::gl::setUniform(FARCLIP, FARCLIP_VALUE);
		putils::gl::setUniform(BIAS, BIAS_VALUE);

		ShaderHelper::shapes::drawQuad();
	}

	void SSAOBlur::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SSAOBlur>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ src::SSAO::Blur::frag, GL_FRAGMENT_SHADER }
		));

		initTexture(_fbo, _colorBuffer, screenWidth, screenHeight);
		texturesToDebug.push_back(Texture{ "SSAO Blur", _colorBuffer });

		_ssaoTextureID = (GLuint)firstTextureID;
		putils::gl::setUniform(ssao, _ssaoTextureID);
	}

	void SSAOBlur::run(size_t ssaoTexture, size_t screenWidth, size_t screenHeight) {
		ShaderHelper::BindFramebuffer __f(_fbo);
		glClear(GL_COLOR_BUFFER_BIT);

		use();
		glActiveTexture(GL_TEXTURE0 + _ssaoTextureID);
		glBindTexture(GL_TEXTURE_2D, (GLuint)ssaoTexture);
		putils::gl::setUniform(this->screenSize, glm::vec2(screenWidth, screenHeight));

		ShaderHelper::shapes::drawQuad();
	}
}