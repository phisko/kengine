#version 330

uniform vec3 direction;

vec3 getLightDirection(vec3 worldPos) {
    return -direction;
}