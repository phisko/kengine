#pragma once

#define MACRO_AS_STRING_IMPL(macro) #macro
#define MACRO_AS_STRING(macro) MACRO_AS_STRING_IMPL(macro)

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

const int MAX_BONES = )" MACRO_AS_STRING(KENGINE_SKELETON_MAX_BONES) R"(;
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
layout (location = 2) out vec3 gcolor;
layout (location = 3) out float gentityID;

uniform int hasTexture;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec3 diffuseColor;
uniform vec3 specularColor;

uniform float entityID;

void main() {
	gposition = WorldPosition;
	gnormal = Normal;
	gentityID = entityID;

	if (hasTexture == 0)
		gcolor = diffuseColor;
	else
		gcolor = texture(texture_diffuse, TexCoords).xyz;
}
	)";
		}
	}
}

