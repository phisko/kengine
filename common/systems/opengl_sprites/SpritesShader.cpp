#include "SpritesShader.hpp"

#include "EntityManager.hpp"

#include "components/TextureModelComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/OpenGLModelComponent.hpp"
#include "components/SpriteComponent.hpp"

#include "systems/opengl/shaders/shaders.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"
#include "helpers/math.hpp"

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
layout (location = 2) out vec4 gcolor;
layout (location = 3) out float gentityID;

void applyTransparency(float alpha);

void main() {
	vec4 pix = texture(tex, TexCoords);
	pix *= color;

	applyTransparency(pix.a);

    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
	gcolor = vec4(pix.rgb, 1.0); // don't apply lighting
	gentityID = entityID;
}
        )";

namespace kengine {
	static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	SpritesShader::SpritesShader(kengine::EntityManager & em)
		: Program(false, pmeta_nameof(SpritesShader)),
		_em(em)
	{}

	void SpritesShader::init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) {
		initWithShaders<SpritesShader>(putils::make_vector(
			ShaderDescription{ vert, GL_VERTEX_SHADER },
			ShaderDescription{ frag, GL_FRAGMENT_SHADER },
			ShaderDescription{ kengine::Shaders::src::ApplyTransparency::frag, GL_FRAGMENT_SHADER }
		));

		_textureID = firstTextureID;
		putils::gl::setUniform(tex, _textureID);
	}

	struct Uniforms {
		size_t textureID;
		GLint color;
		GLint model;
	};

	static void drawObject(EntityManager & em, const GraphicsComponent & graphics, const TransformComponent3f & transform, Uniforms uniforms, bool in2D = false) {
		if (graphics.model == kengine::Entity::INVALID_ID)
			return;

		const auto & modelEntity = em.getEntity(graphics.model);
		if (!modelEntity.has<TextureModelComponent>())
			return;
		const auto texture = modelEntity.get<TextureModelComponent>().texture;

		putils::gl::setUniform(uniforms.color, graphics.color);

		glBindTexture(GL_TEXTURE_2D, texture);

		{
			glm::mat4 model(1.f);

			auto centre = toVec(transform.boundingBox.position);
			if (in2D) {
				centre.y = 1 - centre.y;
				model = glm::translate(model, glm::vec3(-1.f, -1.f, 0.f));
				centre *= 2.f;
			}

			model = glm::translate(model, centre);

			if (in2D)
				model = glm::scale(model, toVec(transform.boundingBox.size));

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

			if (!in2D) {
				model = glm::scale(model, toVec(transform.boundingBox.size));
				model = glm::scale(model, { -1.f, 1.f, 1.f });
			}

			model = glm::scale(model, { 1.f, -1.f, 1.f });

			putils::gl::setUniform(uniforms.model, model);
		}

		ShaderHelper::shapes::drawTexturedQuad();
	}

	void SpritesShader::run(const Parameters & params) {
		const Uniforms uniforms{ _textureID, color, model };

		use();
		
		putils::gl::setUniform(viewPos, params.camPos);
		glActiveTexture((GLenum)(GL_TEXTURE0 + uniforms.textureID));

		putils::gl::setUniform(this->view, glm::mat4(1.f));
		putils::gl::setUniform(this->proj, glm::mat4(1.f));
		for (const auto &[e, graphics, transform, sprite] : _em.getEntities<GraphicsComponent, TransformComponent3f, SpriteComponent2D>()) {
			putils::gl::setUniform(entityID, (float)e.id);
			drawObject(_em, graphics, transform, uniforms, true);
		}

		putils::gl::setUniform(this->view, params.view);
		putils::gl::setUniform(this->proj, params.proj);
		for (const auto &[e, graphics, transform, sprite] : _em.getEntities<GraphicsComponent, TransformComponent3f, SpriteComponent3D>()) {
			putils::gl::setUniform(entityID, (float)e.id);
			drawObject(_em, graphics, transform, uniforms);
		}
	}
}
