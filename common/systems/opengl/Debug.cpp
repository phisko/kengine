#include "Debug.hpp"
#include "EntityManager.hpp"

#include "components/DebugGraphicsComponent.hpp"
#include "components/TransformComponent.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "shaders/ApplyTransparencySrc.hpp"

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
layout (location = 2) out vec4 gcolor;
layout (location = 3) out float gentityID;

void applyTransparency(float alpha);

void main() {
	applyTransparency(color.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gcolor = vec4(color.rgb, 1.0); // don't apply lighting
	gentityID = entityID;
}
)";

namespace kengine::Shaders {
	Debug::Debug(kengine::EntityManager & em)
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

		for (const auto &[e, debug, transform] : _em.getEntities<kengine::DebugGraphicsComponent, kengine::TransformComponent3f>()) {
			_color = debug.color;
			_entityID = (float)e.id;

			if (debug.debugType == DebugGraphicsComponent::Line) {
				glm::mat4 model(1.f);
				model = glm::translate(model, ShaderHelper::toVec(transform.boundingBox.position));
				model = glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });
				model = glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
				model = glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
				_model = model;

				ShaderHelper::shapes::drawLine(ShaderHelper::toVec(debug.offset.position), ShaderHelper::toVec(debug.lineEnd));
			}
			else if (debug.debugType == DebugGraphicsComponent::Sphere || debug.debugType == DebugGraphicsComponent::Box) {
				glm::mat4 model(1.f);
				model = glm::translate(model, ShaderHelper::toVec(transform.boundingBox.position + debug.offset.position));
				model = glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });
				model = glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
				model = glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
				model = glm::scale(model, ShaderHelper::toVec(transform.boundingBox.size * debug.offset.size));
				_model = model;

				if (debug.debugType == DebugGraphicsComponent::Box)
					ShaderHelper::shapes::drawCube();
				else
					ShaderHelper::shapes::drawSphere();
			}
			else if (debug.debugType == DebugGraphicsComponent::Text) {
				// TODO
			}
			else
				assert(!"Unsupported DebugGraphicsComponent type"); // Unsupported type
		}
	}
}