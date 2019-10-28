#pragma once

#include "GL/glew.h"
#include "GL/GL.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "components/ModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"
#include "components/TransformComponent.hpp"

namespace kengine {
	namespace ShaderHelper {
		namespace shapes {
			void drawSphere();
			void drawQuad();
			void drawTexturedQuad();
			void drawCone(const glm::vec3 & dir, float radius);
			void drawCube();
			void drawLine(const glm::vec3 & from, const glm::vec3 & to);
		}

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
		void drawModel(const kengine::OpenGLModelComponent & openGL);
		glm::mat4 getModelMatrix(const kengine::ModelComponent & modelInfo, const kengine::TransformComponent3f & transform);
	}
}