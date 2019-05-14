#include "Debug.hpp"
#include "EntityManager.hpp"

#include "components/DebugGraphicsComponent.hpp"
#include "components/TransformComponent.hpp"

#include "helpers/ShaderHelper.hpp"
#include "shaders/shaders.hpp"
#include "Shapes.hpp"

static const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

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
uniform vec3 color;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec3 gcolor;
layout (location = 3) out float gentityID;

void main() {
    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gcolor = color;
	gentityID = entityID;
}
)";

namespace kengine::Shaders {
	Debug::Debug(kengine::EntityManager & em)
		: Program(false, pmeta_nameof(Debug)),
		_em(em)
	{
	}

	void Debug::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<Debug>(putils::make_vector(
			ShaderDescription{ src::ProjViewModel::vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER }
		));
	}

	void Debug::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
		use();

		glEnable(GL_DEPTH_TEST);

		putils::gl::setUniform(this->view, view);
		putils::gl::setUniform(this->proj, proj);

		for (const auto &[e, debug, transform] : _em.getEntities<kengine::DebugGraphicsComponent, kengine::TransformComponent3f>()) {
			putils::gl::setUniform(this->color, debug.colorNormalized);
			putils::gl::setUniform(this->entityID, (float)e.id);

			glm::mat4 model(1.f);

			model = glm::translate(model, ShaderHelper::toVec(transform.boundingBox.topLeft + debug.offset.topLeft));

			switch (debug.debugType) {
			case DebugGraphicsComponent::Text:
				break;
			case DebugGraphicsComponent::Line:
				break;
			case DebugGraphicsComponent::Sphere:
				model = glm::scale(model, ShaderHelper::toVec({
					transform.boundingBox.size.x * debug.offset.size.x,
					transform.boundingBox.size.y * debug.offset.size.y,
					transform.boundingBox.size.z * debug.offset.size.z
					}));
				putils::gl::setUniform(this->model, model);
				shapes::drawSphere();
				break;
			case DebugGraphicsComponent::Box:
				model = glm::scale(model, ShaderHelper::toVec({
					transform.boundingBox.size.x * debug.offset.size.x,
					transform.boundingBox.size.y * debug.offset.size.y,
					transform.boundingBox.size.z * debug.offset.size.z
					}));

				model = glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
				model = glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
				putils::gl::setUniform(this->model, model);

				shapes::drawCube();
				break;
			default:
				assert(false && "[DebugShader] Unknown debug type");
			}
		}
	}
}