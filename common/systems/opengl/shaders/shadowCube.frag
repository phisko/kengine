#version 330

uniform samplerCube shadowMap;
uniform mat4 lightSpaceMatrix;
uniform vec3 position;
uniform float farPlane;

vec2 getShadowMapValue(vec3 worldPos) {
    vec3 lightDir = worldPos - position;
    float shadowMapValue = texture(shadowMap, lightDir).r * farPlane;
    float objectDepth = length(lightDir);
    return vec2(shadowMapValue, objectDepth);
}

float calcShadow(vec3 worldPos, vec3 normal, vec3 lightDir) {
    vec2 depths = getShadowMapValue(worldPos);
    float shadowMapValue = depths.x;
    float objectDepth = depths.y;

    float bias = 0.05;
    return objectDepth - bias > shadowMapValue ? 1.0 : 0.0;
}