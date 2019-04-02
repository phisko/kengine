namespace kengine::Shaders::src {
    namespace Geometry {
        const char * vert = R"(
#version 330

in vec3 position;
in vec3 color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

out vec4 WorldPosition;
out vec3 EyeRelativePos;
out vec3 Color;

void main() {
	WorldPosition = model * vec4(position, 1.0);
	EyeRelativePos = WorldPosition.xyz - viewPos;
	Color = color;

	gl_Position = proj * view * WorldPosition;
}
        )";

        const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec3 Color;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec3 gcolor;

void main() {
    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gcolor = Color;
}
        )";
    }
}