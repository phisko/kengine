#ifndef KENGINE_NDEBUG

#include "RecastDebugShader.hpp"
#include "kengine.hpp"

#include <RecastDebugDraw.h>
#include <DetourDebugDraw.h>

#include "data/AdjustableComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/GraphicsComponent.hpp"

#include "functions/Execute.hpp"

#include "imgui.h"
#include "helpers/instanceHelper.hpp"
#include "helpers/assertHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/cameraHelper.hpp"
#include "systems/opengl/shaders/shaderHelper.hpp"
#include "systems/opengl/shaders/ApplyTransparencySrc.hpp"

#include "opengl/RAII.hpp"

#include "RecastNavMeshComponent.hpp"

#pragma region GLSL
static const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

out vec4 WorldPosition;
out vec3 EyeRelativePos;
out vec4 Color;

void main() {
	WorldPosition = model * vec4(position, 1.0);
	EyeRelativePos = WorldPosition.xyz - viewPos;
	Color = color;
    gl_Position = proj * view * WorldPosition;
}
)";

static const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec4 Color;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gdiffuse;
layout (location = 3) out vec4 gspecular;
layout (location = 4) out vec4 gentityid;

void applyTransparency(float alpha);

void main() {
	applyTransparency(Color.a);

    gposition = vec4(0.0);
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gdiffuse = vec4(Color.rgb, 1.0); // don't apply lighting
	gspecular = vec4(0.0);
	gentityid = vec4(0.0);
}
)";
#pragma endregion GLSL

namespace kengine {
	static struct {
		bool enabled = false;
		std::string fileName;
	} g_adjustables;

	RecastDebugShader::RecastDebugShader() noexcept
		: Program(false, putils_nameof(RecastDebugShader))
	{
		entities += [](Entity & e) noexcept {
			e += AdjustableComponent{
				"Recast", {
					{ "Debug navmesh", &g_adjustables.enabled }
				}
			};
		};
	}

#pragma region Program
	void RecastDebugShader::init(size_t firstTexture) noexcept {
		initWithShaders<RecastDebugShader>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ opengl::shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		putils::gl::setVertexType<Vertex>();
	}

	void RecastDebugShader::run(const Parameters & params) noexcept {
		if (!g_adjustables.enabled)
			return;

		use();

		shaderHelper::Enable blend(GL_BLEND);
		
		glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // position
		glBlendFunci(4, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // entityID

		_view = params.view;
		_proj = params.proj;
		_viewPos = params.camPos;

		for (const auto & [e, graphics, instance, transform] : entities.with<GraphicsComponent, InstanceComponent, TransformComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto model = entities[instance.model];
			const auto recastNavMesh = model.tryGet<RecastNavMeshComponent>();
			if (!recastNavMesh)
				continue;

			_model = matrixHelper::getModelMatrix(transform, model.tryGet<TransformComponent>());

			duDebugDrawNavMesh(this, *recastNavMesh->navMesh, 0);
		}

		glBlendFunci(0, GL_ONE, GL_ZERO); // position
		glBlendFunci(4, GL_ONE, GL_ZERO); // entityID
	}
#pragma endregion Program

#pragma region duDebugDraw
	void RecastDebugShader::begin(duDebugDrawPrimitives prim, float size) {
		switch (prim) {
		case duDebugDrawPrimitives::DU_DRAW_LINES:
			_currentVertexType = GL_LINES;
			break;
		case duDebugDrawPrimitives::DU_DRAW_POINTS:
			_currentVertexType = GL_POINTS;
			break;
		case duDebugDrawPrimitives::DU_DRAW_QUADS:
			_currentVertexType = GL_QUADS;
			break;
		case duDebugDrawPrimitives::DU_DRAW_TRIS:
			_currentVertexType = GL_TRIANGLES;
			break;
		default:
			kengine_assert_failed("Unknown primitive type");
		}
	}

	void RecastDebugShader::vertex(const float x, const float y, const float z, unsigned int color) {
		Vertex vertex;

		vertex.pos[0] = x;
		vertex.pos[1] = y;
		vertex.pos[2] = z;

		const auto normalizedColor = putils::fromRGBA(color);
		memcpy(vertex.color, normalizedColor.attributes, sizeof(vertex.color));

		_currentMesh.push_back(vertex);
	}

	void RecastDebugShader::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v) {
		vertex(x, y, z, color);
	}

	void RecastDebugShader::end() {
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, _currentMesh.size() * sizeof(Vertex), _currentMesh.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(_currentVertexType, 0, (GLsizei)_currentMesh.size());
		_currentMesh.clear();
	}
#pragma endregion duDebugDraw
}

#endif
