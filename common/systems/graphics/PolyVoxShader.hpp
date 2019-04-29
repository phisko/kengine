#pragma once

#include "EntityManager.hpp"

#include "components/TransformComponent.hpp"
#include "components/PolyVoxComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"
#include "opengl/Program.hpp"

namespace kengine {
	class PolyVoxShader : public putils::gl::Program {
		static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	public:
		PolyVoxShader(kengine::EntityManager & em) : Program(false, pmeta_nameof(PolyVoxShader)), _em(em) {}

		void init(size_t firstTextureID, size_t screenWidth, size_t screenHeight, GLuint gBufferFBO) override {
			initWithShaders<PolyVoxShader>(putils::make_vector(
				ShaderDescription{ vert, GL_VERTEX_SHADER },
				ShaderDescription{ frag, GL_FRAGMENT_SHADER }
			));
		}

		void run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) override {
			use();

			glEnable(GL_DEPTH_TEST);

			putils::gl::setUniform(this->view, view);
			putils::gl::setUniform(this->proj, proj);
			putils::gl::setUniform(viewPos, camPos);

			for (const auto &[e, poly, modelComp, transform] : _em.getEntities<PolyVoxModelComponent, kengine::ModelComponent, kengine::TransformComponent3f>()) {
				const auto & modelInfoEntity = _em.getEntity(modelComp.modelInfo);
				if (!modelInfoEntity.has<kengine::ModelInfoComponent>())
					continue;
				const auto & modelInfo = modelInfoEntity.get<kengine::ModelInfoComponent>();

				glm::mat4 model(1.f);
				const auto & centre = transform.boundingBox.topLeft;
				model = glm::translate(model, toVec(centre));
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
					modelInfo.yaw,
					{ 0.f, 1.f, 0.f }
				);

				model = glm::rotate(model,
					modelInfo.pitch,
					{ 1.f, 0.f, 0.f }
				);

				model = glm::translate(model, -modelInfo.translation); // Re-center
				model = glm::scale(model, modelInfo.scale);

				putils::gl::setUniform(this->model, model);

				for (const auto & meshInfo : modelInfo.meshes) {
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
		GLint viewPos;

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&PolyVoxShader::model),
			pmeta_reflectible_attribute(&PolyVoxShader::view),
			pmeta_reflectible_attribute(&PolyVoxShader::proj),
			pmeta_reflectible_attribute(&PolyVoxShader::viewPos)
		);

	private:
		kengine::EntityManager & _em;

	private:
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

	gl_Position = proj * view * WorldPosition;
}
		)";

		static inline const char * frag = R"(
#version 330

in vec4 WorldPosition;
in vec3 EyeRelativePos;
in vec3 Color;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec3 gcolor;

void main() {
    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gcolor = Color;
}
        )";

	};
}