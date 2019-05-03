#include "PolyVoxShader.hpp"

#include "EntityManager.hpp"

#include "components/TransformComponent.hpp"
#include "components/PolyVoxComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"

#include "helpers/ShaderHelper.hpp"

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

uniform float entityID;

layout (location = 0) out vec4 gposition;
layout (location = 1) out vec3 gnormal;
layout (location = 2) out vec3 gcolor;
layout (location = 3) out float gentityID;

void main() {
    gposition = WorldPosition;
    gnormal = -normalize(cross(dFdy(EyeRelativePos), dFdx(EyeRelativePos)));
    gcolor = Color;
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
			ShaderDescription{ frag, GL_FRAGMENT_SHADER }
		));
	}

	void PolyVoxShader::run(const glm::mat4 & view, const glm::mat4 & proj, const glm::vec3 & camPos, size_t screenWidth, size_t screenHeight) {
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

			putils::gl::setUniform(this->model, ShaderHelper::getModelMatrix(modelInfo, transform));
			putils::gl::setUniform(this->entityID, (float)e.id);

			for (const auto & meshInfo : modelInfo.meshes) {
				glBindVertexArray(meshInfo.vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
			}
		}

		glDisable(GL_DEPTH_TEST);
	}
}
