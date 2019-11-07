#include "PolyVoxShader.hpp"

#include "EntityManager.hpp"

#include "components/TransformComponent.hpp"
#include "components/PolyVoxComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"

#include "common/systems/opengl/shaders/shaders.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

static inline const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 color;

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
	//Color = vec3(1.0); // This is pretty

	gl_Position = proj * view * WorldPosition;
}
		)";

static inline const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec3 Color;

uniform vec4 color;
uniform float entityID;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gcolor;
layout (location = 3) out float gentityID;

void applyTransparency(float a);

void main() {
	applyTransparency(color.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gcolor = vec4(Color * color.rgb, 0.0);
	gentityID = entityID;
}
        )";

namespace kengine {
	static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	PolyVoxShader::PolyVoxShader(kengine::EntityManager & em)
		: Program(false, pmeta_nameof(PolyVoxShader)),
		_em(em)
	{}

	void PolyVoxShader::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<PolyVoxShader>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ kengine::Shaders::src::ApplyTransparency::frag, GL_FRAGMENT_SHADER }
		));
	}

	void PolyVoxShader::run(const Parameters & params) {
		use();

		_view = params.view;
		_proj = params.proj;
		_viewPos = params.camPos;

		for (const auto &[e, poly, graphics, transform] : _em.getEntities<PolyVoxObjectComponent, GraphicsComponent, TransformComponent3f>()) {
			if (graphics.model == kengine::Entity::INVALID_ID)
				continue;

			const auto & modelInfoEntity = _em.getEntity(graphics.model);
			if (!modelInfoEntity.has<OpenGLModelComponent>() || !modelInfoEntity.has<ModelComponent>())
				continue;

			const auto & modelInfo = modelInfoEntity.get<ModelComponent>();
			const auto & openGL = modelInfoEntity.get<OpenGLModelComponent>();

			_model = ShaderHelper::getModelMatrix(modelInfo, transform);
			_entityID = (float)e.id;
			_color = graphics.color;

			ShaderHelper::drawModel(openGL);
		}
	}
}
