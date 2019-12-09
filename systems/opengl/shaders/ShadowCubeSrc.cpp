namespace kengine::Shaders::src {
	namespace ShadowCube {
		namespace Frag {
			const char * glsl = R"(
#version 330

uniform samplerCube shadowMap;
uniform vec3 position;
uniform vec3 viewPos;
uniform float farPlane;
uniform float bias;

vec2 getShadowMapValue(vec3 worldPos) {
    vec3 lightDir = worldPos - position;
    float shadowMapValue = texture(shadowMap, lightDir).r * farPlane;
    float objectDepth = length(lightDir);
    return vec2(shadowMapValue, objectDepth);
}

float calcShadow(vec3 worldPos, vec3 normal, vec3 lightDirIgnore) {
	vec3 lightDir = worldPos - position;
	float currentDepth = length(lightDir);

	vec3 sampleOffsetDirections[20] = vec3[](
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);   
	const int samples = 20;

	float viewDistance = length(viewPos - worldPos);
	float sampleRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
	
	float shadow = 0.0;
	for (int i = 0; i < samples; ++i) {
		float closestDepth = texture(shadowMap, lightDir + sampleOffsetDirections[i] * sampleRadius).r;
		closestDepth *= farPlane;
		if (currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

	return shadow;
}
        )";
		}
	}
}