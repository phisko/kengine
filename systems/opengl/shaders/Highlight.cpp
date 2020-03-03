#include "Highlight.hpp"
#include "EntityManager.hpp"

#include "data/HighlightComponent.hpp"

#include "QuadSrc.hpp"
#include "ShaderHelper.hpp"

static const auto frag = R"(
#version 330

uniform vec3 viewPos;
uniform vec2 screenSize;

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D gentityID;

uniform float entityID;
uniform vec4 highlightColor;
uniform float intensity;

out vec4 outColor;

void main() {
	outColor = vec4(0.0);

   	vec2 texCoord = gl_FragCoord.xy / screenSize;

    vec4 worldPos = texture(gposition, texCoord);
	if (worldPos.w == 0) // Empty pixel
		return;

	vec3 normal = -texture(gnormal, texCoord).xyz;
	if (texture(gentityID, texCoord).x != entityID)
		return;

	float highlight = 0.0;
	float distanceToEntity = length(worldPos.xyz - viewPos);

	for (float x = -intensity; x <= intensity; x += 1.0)
		for (float y = -intensity; y <= intensity; y += 1.0) {
			vec2 neighborCoord = vec2(gl_FragCoord.x + x, gl_FragCoord.y + y) / screenSize;
	
			vec4 neighborPos = texture(gposition, neighborCoord);
			bool emptyNeighbor = neighborPos.w == 0;
			bool neighborCloser = length(neighborPos.xyz - viewPos) < distanceToEntity;
			if (!emptyNeighbor && neighborCloser) // This pixel is an object closer than entityID
				continue;

			if (texture(gentityID, neighborCoord).x != entityID)
				highlight = 1.0;
		}

	outColor = highlight * highlightColor;
}
)";

namespace kengine::Shaders {
	Highlight::Highlight(EntityManager & em)
		: Program(true, putils_nameof(Highlight)),
		_em(em)
	{
	}

	void Highlight::init(size_t firstTextureID) {
		initWithShaders<Highlight>(putils::make_vector(
			ShaderDescription{ src::Quad::Vert::glsl, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER }
		));
	}

	void Highlight::run(const Parameters & params) {
		use();

		ShaderHelper::Enable _(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		_viewPos = params.camPos;
		_screenSize = putils::Point2f(params.viewport.size);

		for (const auto & [e, highlight] : _em.getEntities<HighlightComponent>()) {
			_entityID = (float)e.id;
			_highlightColor = highlight.color;
			_intensity = highlight.intensity * 2.f - 1.f; // convert from [0,1] to [-1,1]
			ShaderHelper::shapes::drawQuad();
		}
	}
}