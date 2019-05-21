#include "Highlight.hpp"
#include "EntityManager.hpp"

#include "components/HighlightComponent.hpp"
#include "components/AdjustableComponent.hpp"

#include "Shapes.hpp"
#include "shaders/shaders.hpp"
#include "helpers/ShaderHelper.hpp"

static const auto frag = R"(
#version 330

uniform vec3 viewPos;
uniform vec2 screenSize;

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gentityID;

uniform int HIGHLIGHT_ALL;

uniform float entityID;
uniform vec3 highlightColor;
uniform float intensity;

out vec4 outColor;

void main() {
   	vec2 texCoord = gl_FragCoord.xy / screenSize;

    vec4 worldPos = texture(gposition, texCoord);
	if (worldPos.w == 0) { // Empty pixel
		outColor = vec4(0.0);
		return;
	}

	vec3 normal = -texture(gnormal, texCoord).xyz;
	if (texture(gentityID, texCoord).x != entityID && HIGHLIGHT_ALL == 0) {
		outColor = vec4(0.0);
		return;
	}

	float highlight = 1.0 - dot(normalize(normal), normalize(worldPos.xyz - viewPos)) + intensity;
	outColor = highlight * vec4(highlightColor, 1.0);
}
)";

static bool HIGHLIGHT_ALL_ADJUST = false;
static float HIGHLIGHT_ALL_R = 0.f;
static float HIGHLIGHT_ALL_G = 0.f;
static float HIGHLIGHT_ALL_B = 0.f;

static float INTENSITY_ALL = .5f;

namespace kengine::Shaders {
	Highlight::Highlight(kengine::EntityManager & em)
		: Program(true, pmeta_nameof(Highlight)),
		_em(em)
	{
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Highlight] Highlight all", &HIGHLIGHT_ALL_ADJUST); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Highlight] 1. R", &HIGHLIGHT_ALL_R); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Highlight] 2. G", &HIGHLIGHT_ALL_G); };
		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Highlight] 3. B", &HIGHLIGHT_ALL_B); };

		em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Highlight] Intensity", &INTENSITY_ALL); };
	}

	void Highlight::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<Highlight>(putils::make_vector(
			ShaderDescription{ src::Quad::vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER }
		));
	}

	void Highlight::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		use();
		putils::gl::setUniform(this->viewPos, camPos);
		putils::gl::setUniform(this->screenSize, glm::vec2(screenWidth, screenHeight));

		putils::gl::setUniform(this->HIGHLIGHT_ALL, HIGHLIGHT_ALL_ADJUST ? GLuint(1) : 0);
		if (HIGHLIGHT_ALL_ADJUST) {
			putils::gl::setUniform(this->highlightColor, glm::vec3(HIGHLIGHT_ALL_R, HIGHLIGHT_ALL_G, HIGHLIGHT_ALL_B));
			putils::gl::setUniform(this->intensity, INTENSITY_ALL * 2.f - 1.f); // convert from [0,1] to [-1,1]
			shapes::drawQuad();
			return;
		}

		for (const auto & [e, highlight] : _em.getEntities<HighlightComponent>()) {
			putils::gl::setUniform(this->entityID, (float)e.id);
			putils::gl::setUniform(this->highlightColor, ShaderHelper::toVec(highlight.colorNormalized));
			putils::gl::setUniform(this->intensity, highlight.intensity * 2.f - 1.f); // convert from [0,1] to [-1,1]
			shapes::drawQuad();
		}
	}
}