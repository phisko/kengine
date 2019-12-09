#pragma once

#include "macro_as_string.hpp"

namespace kengine {
	namespace src {
		namespace TexturedShader {
			static const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec4 boneWeights;
layout (location = 4) in ivec4 boneIDs;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = )" putils_macro_as_string(KENGINE_SKELETON_MAX_BONES) R"(;
uniform mat4 bones[MAX_BONES];

out vec4 WorldPosition;
out vec3 Normal;
out vec2 TexCoords;

void main() {
	mat4 boneMatrix = bones[boneIDs[0]] * boneWeights[0];
	boneMatrix += bones[boneIDs[1]] * boneWeights[1];
	boneMatrix += bones[boneIDs[2]] * boneWeights[2];
	boneMatrix += bones[boneIDs[3]] * boneWeights[3];

	WorldPosition = model * boneMatrix * vec4(position, 1.0);
	Normal = (boneMatrix * vec4(normal, 0.0)).xyz;
	TexCoords = texCoords;

	gl_Position = proj * view * WorldPosition;
}
	)";

			static const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 Normal;
in vec2 TexCoords;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gcolor;
layout (location = 3) out float gentityID;

uniform int hasTexture;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec4 diffuseColor;
uniform vec4 specularColor;

uniform float entityID;
uniform vec4 color;

void applyTransparency(float a);

void main() {
	vec4 totalColor;
	if (hasTexture == 0)
		totalColor = diffuseColor;
	else
		totalColor = texture(texture_diffuse, TexCoords);

	totalColor *= color;

	applyTransparency(totalColor.a);

	gposition = WorldPosition;
	gnormal = Normal;
	gentityID = entityID;
	gcolor = vec4(totalColor.xyz, 0.0);
}
	)";
		}
	}
}


