#version 330

uniform vec3 position;

vec3 getLightDirection(vec3 worldPos) {
    return normalize(position - worldPos);
}