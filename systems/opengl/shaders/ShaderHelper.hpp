#pragma once

#include "GL/glew.h"
#include "GL/GL.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data/ModelComponent.hpp"
#include "data/OpenGLModelComponent.hpp"
#include "data/TransformComponent.hpp"

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
			BindFramebuffer(GLint fbo) : fbo(fbo) {
				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			}
			~BindFramebuffer() { glBindFramebuffer(GL_FRAMEBUFFER, prev); }

			GLint fbo;
			GLint prev;
		};

		struct Enable {
			Enable(GLenum feature) : feature(feature) { glEnable(feature); }
			~Enable() { glDisable(feature); }

			GLenum feature;
		};

		static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
		void drawModel(const OpenGLModelComponent & openGL);
		glm::mat4 getModelMatrix(const ModelComponent & modelInfo, const TransformComponent & transform);
	}
}