#ifndef KENGINE_NDEBUG

#include "RecastDebugShader.hpp"
#include "EntityManager.hpp"

#include <RecastDebugDraw.h>
#include <DetourDebugDraw.h>

#include "data/AdjustableComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/ImGuiComponent.hpp"

#include "imgui.h"
#include "helpers/instanceHelper.hpp"
#include "helpers/assertHelper.hpp"
#include "systems/opengl/shaders/ApplyTransparencySrc.hpp"
#include "systems/opengl/shaders/shaderHelper.hpp"

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
layout (location = 4) out float gentityid;

void applyTransparency(float alpha);

void main() {
	applyTransparency(Color.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gdiffuse = vec4(Color.rgb, 1.0); // don't apply lighting
	gspecular = vec4(0.0);
	gentityid = 0.0;
}
)";

namespace kengine {
	static struct {
		bool enabled = false;
		std::string fileName;
	} g_adjustables;

	RecastDebugShader::RecastDebugShader(EntityManager & em)
		: Program(false, putils_nameof(RecastDebugShader)),
		_em(em)
	{
		em += [&](Entity & e) {
			e += AdjustableComponent{
				"Navmesh", {
					{ "Debug", &g_adjustables.enabled }
				}
			};

			e += ImGuiComponent([&] {
				if (ImGui::Begin("RecastShader")) {
					if (ImGui::BeginCombo("File", g_adjustables.fileName.c_str())) {
						for (const auto & [e, model, recast] : em.getEntities<ModelComponent, RecastComponent>())
							if (ImGui::Selectable(model.file))
								g_adjustables.fileName = model.file;
						ImGui::EndCombo();
					}
				}
				ImGui::End();
			});
		};
	}

	void RecastDebugShader::init(size_t firstTexture) {
		initWithShaders<RecastDebugShader>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ Shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		putils::gl::setVertexType<Vertex>();
	}

	void RecastDebugShader::run(const Parameters & params) {
		use();

		_view = params.view;
		_proj = params.proj;
		_viewPos = params.camPos;

		for (const auto & [e, graphics, instance, transform] : _em.getEntities<GraphicsComponent, InstanceComponent, TransformComponent>()) {
			if (graphics.appearance != g_adjustables.fileName)
				continue;

			if (!shaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			const auto model = _em.getEntity(instance.model);
			_model = shaderHelper::getModelMatrix(model.get<ModelComponent>(), transform);

			const auto & comp = model.get<RecastComponent>();
			duDebugDrawNavMesh(this, *comp.navMesh, 0);
		}
	}

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
			kengine_assert_failed(_em, "Unknown primitive type");
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
		glDrawArrays(_currentVertexType, 0, _currentMesh.size());
		_currentMesh.clear();
	}
}

#endif
