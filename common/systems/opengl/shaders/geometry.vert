#version 330

in vec3 position;
in vec3 color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

out vec3 WorldPosition;
out vec3 EyeRelativePos;
out vec3 Color;

void main() {
	WorldPosition = (model * vec4(position, 1.0)).xyz;
	EyeRelativePos = WorldPosition - viewPos;
	Color = color;

	gl_Position = proj * view * vec4(WorldPosition, 1.0);
}