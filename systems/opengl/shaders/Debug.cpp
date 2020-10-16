#include "Debug.hpp"
#include "EntityManager.hpp"

#include "data/DebugGraphicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "shaderHelper.hpp"
#include "ApplyTransparencySrc.hpp"

#include "visit.hpp"
#include "static_assert.hpp"

#pragma region GLSL
static const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

out vec4 WorldPosition;
out vec3 EyeRelativePos;

void main() {
	WorldPosition = model * vec4(position, 1.0);
	EyeRelativePos = WorldPosition.xyz - viewPos;
    gl_Position = proj * view * WorldPosition;
}
)";


static const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 EyeRelativePos;

uniform float entityID;
uniform vec4 color;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gdiffuse;
layout (location = 3) out vec4 gspecular;
layout (location = 4) out float gentityID;

void applyTransparency(float alpha);

void main() {
	applyTransparency(color.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gdiffuse = vec4(color.rgb, 1.0); // don't apply lighting
	gspecular = vec4(0.0);
	gentityID = entityID;
}
)";
#pragma endregion GLSL

namespace kengine::opengl::shaders {
	Debug::Debug(EntityManager & em)
		: Program(false, putils_nameof(Debug)),
		_em(em)
	{
	}

	void Debug::init(size_t firstTextureID) {
		initWithShaders<Debug>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));
	}

	void Debug::run(const Parameters & params) {
		use();

		_view = params.view;
		_proj = params.proj;
		_viewPos = params.camPos;

		const auto commonMatrixTransform = [](glm::mat4 & model, const kengine::TransformComponent & transform, DebugGraphicsComponent::ReferenceSpace referenceSpace) {
			if (referenceSpace == DebugGraphicsComponent::ReferenceSpace::World)
				return;
			model = glm::translate(model, shaderHelper::toVec(transform.boundingBox.position));
			model = glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
			model = glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
			model = glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });
		};

		for (const auto &[e, debug, transform] : _em.getEntities<DebugGraphicsComponent, TransformComponent>()) {
			if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			for (const auto & element : debug.elements) {
				_color = element.color;
				_entityID = (float)e.id;

				glm::mat4 model{ 1.f };
				std::visit(putils::overloaded{
					[&](const DebugGraphicsComponent::Line & line) {
						commonMatrixTransform(model, transform, element.referenceSpace);
						_model = model;
						shaderHelper::shapes::drawLine(shaderHelper::toVec(element.pos), shaderHelper::toVec(line.end));
					},
					[&](const DebugGraphicsComponent::Sphere & sphere) {
						model = glm::translate(model, shaderHelper::toVec(element.pos));
						commonMatrixTransform(model, transform, element.referenceSpace);
						model = glm::scale(model, shaderHelper::toVec(transform.boundingBox.size * sphere.radius));
						_model = model;
						shaderHelper::shapes::drawSphere();
					},
					[&](const DebugGraphicsComponent::Box & box) {
						model = glm::translate(model, shaderHelper::toVec(element.pos));
						commonMatrixTransform(model, transform, element.referenceSpace);
						model = glm::scale(model, shaderHelper::toVec(transform.boundingBox.size * box.size));
						_model = model;
						shaderHelper::shapes::drawCube();
					},
					[&](const DebugGraphicsComponent::Text & text) {
						// TODO
					},
					[&](auto && v) {
						static_assert(putils::always_false<decltype(v)>(), "Non exhaustive visitor");
					}
				}, element.data);
			}
		}
	}
}