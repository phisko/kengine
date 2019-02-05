#version 330

in vec3 WorldPosition;
in vec3 EyeRelativePos;
in vec3 Color;

layout (location = 0) out vec3 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec3 gcolor;

void main() {
    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gcolor = Color;
}