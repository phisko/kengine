#pragma once

#include "GL/glew.h"
#include "GL/GL.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "components/ModelInfoComponent.hpp"
#include "components/TransformComponent.hpp"

#ifndef KENGINE_SHADOW_CUBE_SIZE
# define KENGINE_SHADOW_CUBE_SIZE 1024
#endif

#ifndef KENGINE_SHADOW_MAP_SIZE
# define KENGINE_SHADOW_MAP_SIZE 8192
#endif

namespace kengine {
	namespace ShaderHelper {
		struct BindFramebuffer {
			BindFramebuffer(GLuint fbo, GLuint prev = 0) : fbo(fbo), prev(prev) { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
			~BindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, prev); }

			GLuint fbo;
			GLuint prev;
		};

		struct Enable {
			Enable(GLenum feature) : feature(feature) { glEnable(feature); }
			~Enable() { glDisable(feature); }

			GLenum feature;
		};

		static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

		static void drawModel(const kengine::ModelInfoComponent & modelInfo) {
			for (const auto & meshInfo : modelInfo.meshes) {
				glBindVertexArray(meshInfo.vertexArrayObject);
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
			}
		}

		static glm::mat4 getModelMatrix(const kengine::ModelInfoComponent & modelInfo, const kengine::TransformComponent3f & transform) {
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

			model = glm::translate(model, -modelInfo.translation); // Re-center
			model = glm::scale(model, modelInfo.scale);

			model = glm::rotate(model,
				modelInfo.yaw,
				{ 0.f, 1.f, 0.f }
			);

			model = glm::rotate(model,
				modelInfo.pitch,
				{ 1.f, 0.f, 0.f }
			);

			return model;
		}
	}
}