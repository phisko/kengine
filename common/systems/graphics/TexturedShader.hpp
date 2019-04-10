#pragma once

#include "components/TransformComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/TexturedModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	class TexturedShader : public putils::gl::Program {
		static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	public:
		TexturedShader(kengine::EntityManager & em) : Program(false, pmeta_nameof(TexturedShader)), _em(em) {}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override {
			initWithShaders<TexturedShader>(putils::make_vector(
				ShaderDescription{ vert, GL_VERTEX_SHADER },
				ShaderDescription{ frag, GL_FRAGMENT_SHADER }
			));

			_diffuseTextureID = firstTextureID;
			putils::gl::setUniform(texture_diffuse, _diffuseTextureID);

			_specularTextureID = _diffuseTextureID + 1;
			putils::gl::setUniform(texture_specular, _specularTextureID);
		}

		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override {
			use();

			glEnable(GL_DEPTH_TEST);

			putils::gl::setUniform(this->view, view);
			putils::gl::setUniform(this->proj, proj);

			for (const auto &[e, textured, modelComp, transform] : _em.getEntities<TexturedModelComponent, kengine::ModelComponent, kengine::TransformComponent3f>()) {
				const auto & modelInfoEntity = _em.getEntity(modelComp.modelInfo);
				if (!modelInfoEntity.has<kengine::ModelInfoComponent>())
					continue;
				const auto & modelInfo = modelInfoEntity.get<kengine::ModelInfoComponent>();
				const auto & textures = modelInfoEntity.get<kengine::ModelInfoTexturesComponent>();

				glm::mat4 model(1.f);
				const auto & centre = transform.boundingBox.topLeft;
				model = glm::translate(model, toVec(centre));
				model = glm::scale(model, toVec(transform.boundingBox.size));

				model = glm::rotate(model,
					transform.pitch + modelInfo.pitch,
					{ 1.f, 0.f, 0.f }
				);

				model = glm::rotate(model,
					transform.yaw + modelInfo.yaw,
					{ 0.f, 1.f, 0.f }
				);

				model = glm::translate(model, -modelInfo.translation); // Re-center

				putils::gl::setUniform(this->model, model);

				for (unsigned int i = 0; i < modelInfo.meshes.size(); ++i) {
					const auto & meshInfo = modelInfo.meshes[i];
					const auto & meshTextures = textures.meshes[i];

					assert(!meshTextures.diffuse.empty());
					glActiveTexture(GL_TEXTURE0 + _diffuseTextureID);
					glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);

					glActiveTexture(GL_TEXTURE0 + _specularTextureID);
					if (meshTextures.specular.empty())
						glBindTexture(GL_TEXTURE_2D, meshTextures.diffuse[0]);
					else
						glBindTexture(GL_TEXTURE_2D, meshTextures.specular[0]);

					glBindVertexArray(meshInfo.vertexArrayObject);
					glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
					glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
				}
			}

			glDisable(GL_DEPTH_TEST);
		}

	public:
		GLint model;
		GLint view;
		GLint proj;
		GLint texture_diffuse;
		GLint texture_specular;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&TexturedShader::model),
			pmeta_reflectible_attribute(&TexturedShader::view),
			pmeta_reflectible_attribute(&TexturedShader::proj),

			pmeta_reflectible_attribute(&TexturedShader::texture_diffuse),
			pmeta_reflectible_attribute(&TexturedShader::texture_specular)
		);

	private:
		kengine::EntityManager & _em;
		size_t _diffuseTextureID;
		size_t _specularTextureID;

	private:
		static inline const char * vert = R"(
#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoords;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec4 WorldPosition;
out vec3 Normal;
out vec2 TexCoords;

void main() {
	WorldPosition = model * vec4(position, 1.0);
	TexCoords = texCoords;
	Normal = normal;

	gl_Position = proj * view * WorldPosition;
}
	)";

		static inline const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 Normal;
in vec2 TexCoords;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec3 gcolor;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main() {
	gposition = WorldPosition;
	gnormal = Normal;
	gcolor = texture(texture_diffuse, TexCoords).xyz;
}
	)";
	};
}