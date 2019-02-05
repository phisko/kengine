#version 330

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main() {
    float lightDistance = length(FragPos.xyz - lightPos);
    lightDistance /= farPlane;
    gl_FragDepth = lightDistance;
}