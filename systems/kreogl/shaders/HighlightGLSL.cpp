#include "HighlightGLSL.hpp"

const char * HighlightGLSL::frag = R"(
#version 330

uniform vec3 viewPos;
uniform vec2 screenSize;

uniform sampler2D gposition;
uniform sampler2D gnormal;
uniform sampler2D guserData;

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
	if (texture(guserData, texCoord).x != entityID)
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

			if (texture(guserData, neighborCoord).x != entityID)
				highlight = 1.0;
		}

	outColor = highlight * highlightColor;
})";