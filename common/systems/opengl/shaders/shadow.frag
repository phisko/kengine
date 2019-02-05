#version 330

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

float calcShadow(vec3 worldPos, vec3 normal, vec3 lightDir) {
    vec4 worldPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 projCoords = worldPosLightSpace.xyz / worldPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // transform to [0,1] range
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

