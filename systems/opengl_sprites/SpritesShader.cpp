#include "SpritesShader.hpp"

#include "EntityManager.hpp"

#include "data/TextureModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/OpenGLModelComponent.hpp"
#include "data/SpriteComponent.hpp"

#include "systems/opengl/shaders/ApplyTransparencySrc.hpp"
#include "systems/opengl/shaders/shaderHelper.hpp"

#include "helpers/instanceHelper.hpp"
#include "helpers/cameraHelper.hpp"

#pragma region GLSL
static inline const char * vert = R"(
#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 viewPos;

out vec4 WorldPosition;
out vec3 EyeRelativePos;
out vec2 TexCoords;

void main() {
	WorldPosition = model * vec4(position, 1.0);
	EyeRelativePos = WorldPosition.xyz - viewPos;
	TexCoords = texCoords;

	gl_Position = proj * view * WorldPosition;
}
		)";

static inline const char * frag = R"(
#version 330

uniform sampler2D tex;
uniform vec4 color;
uniform float entityID;

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec2 TexCoords;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec4 gdiffuse;
layout (location = 3) out vec4 gspecular;
layout (location = 4) out float gentityID;

void applyTransparency(float alpha);

void main() {
	vec4 pix = texture(tex, TexCoords);
	pix *= color;

	applyTransparency(pix.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gdiffuse = vec4(pix.rgb, 1.0); // don't apply lighting
	gspecular = vec4(0.0); // don't apply lighting
	gentityID = entityID;
}
        )";
#pragma endregion GLSL

namespace kengine {
	SpritesShader::SpritesShader(EntityManager & em)
		: Program(false, putils_nameof(SpritesShader)),
		_em(em)
	{}

	void SpritesShader::init(size_t firstTextureID) {
		initWithShaders<SpritesShader>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ Shaders::src::ApplyTransparency::Frag::glsl, GL_FRAGMENT_SHADER }
		));

		_textureID = firstTextureID;
		_tex = _textureID;
	}

	struct Uniforms {
		size_t textureID;
		putils::gl::Uniform<putils::NormalizedColor> color;
		putils::gl::Uniform<glm::mat4> model;
	};

#pragma region declarations
	static void drawObject(EntityManager & em, const putils::gl::Program::Parameters & params, const GraphicsComponent & graphics, const InstanceComponent & instance, const TransformComponent & transform, Uniforms uniforms, const SpriteComponent2D * comp = nullptr);
#pragma endregion
	void SpritesShader::run(const Parameters & params) {
		const Uniforms uniforms{ _textureID, _color, _model };

		use();
		
		_viewPos = params.camPos;
		glActiveTexture((GLenum)(GL_TEXTURE0 + uniforms.textureID));

		_view = glm::mat4(1.f);
		_proj = glm::mat4(1.f);
		for (const auto &[e, instance, graphics, transform, sprite] : _em.getEntities<InstanceComponent, GraphicsComponent, TransformComponent, SpriteComponent2D>()) {
			if (!shaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_entityID = (float)e.id;
			drawObject(_em, params, graphics, instance, transform, uniforms, &sprite);
		}

		_view = params.view;
		_proj = params.proj;
		for (const auto &[e, instance, graphics, transform, sprite] : _em.getEntities<InstanceComponent, GraphicsComponent, TransformComponent, SpriteComponent3D>()) {
			if (!shaderHelper::entityAppearsInViewport(e, params.viewportID))
				continue;

			_entityID = (float)e.id;
			drawObject(_em, params, graphics, instance, transform, uniforms);
		}
	}

	static void drawObject(EntityManager & em, const putils::gl::Program::Parameters & params, const GraphicsComponent & graphics, const InstanceComponent & instance, const TransformComponent & transform, Uniforms uniforms, const SpriteComponent2D * comp) {
		uniforms.color = graphics.color;

		const auto modelEntity = em.getEntity(instance.model);
		if (!modelEntity.has<TextureModelComponent>())
			return;

		glBindTexture(GL_TEXTURE_2D, modelEntity.get<TextureModelComponent>().texture);

		{
			const auto & box =
				comp == nullptr ?
				transform.boundingBox : // 3D
				cameraHelper::convertToScreenPercentage(transform.boundingBox, params.viewport.size, *comp); // 2D

			auto centre = shaderHelper::toVec(box.position);
			const auto size = shaderHelper::toVec(box.size);

			glm::mat4 model(1.f);

			if (comp != nullptr) {
				centre.y = 1.f - centre.y;
				model = glm::translate(model, glm::vec3(-1.f, -1.f, 0.f));
				centre *= 2.f;
			}

			model = glm::translate(model, centre);

			if (comp != nullptr)
				model = glm::scale(model, size);

			model = glm::rotate(model,
				transform.yaw,
				{ 0.f, 1.f, 0.f }
			);
			model = glm::rotate(model,
				transform.pitch,
				{ 1.f, 0.f, 0.f }
			);
			model = glm::rotate(model,
				transform.roll,
				{ 0.f, 0.f, 1.f }
			);

			if (comp == nullptr) {
				model = glm::scale(model, size);
				model = glm::scale(model, { -1.f, 1.f, 1.f });
			}

			model = glm::scale(model, { 1.f, -1.f, 1.f });

			uniforms.model = model;
		}

		shaderHelper::shapes::drawTexturedQuad();
	}
}
