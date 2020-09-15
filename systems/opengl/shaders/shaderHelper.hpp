#pragma once

#include "GL/glew.h"
#include "GL/GL.h"

#include <cmath>
#include <glm/glm.hpp>

#include "opengl/Mesh.hpp"

#include "data/ModelComponent.hpp"
#include "data/SystemSpecificModelComponent.hpp"
#include "data/TransformComponent.hpp"

namespace kengine {
	namespace shaderHelper {
		inline glm::vec3 toVec(const putils::Point3f & p);
		void drawModel(const SystemSpecificModelComponent<putils::gl::Mesh> & openGL);

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

		// Impl
		inline glm::vec3 toVec(const putils::Point3f & p) {
			return { p.x, p.y, p.z };
		}
	}
}