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
		inline glm::vec3 toVec(const putils::Point3f & p) noexcept;
		void drawModel(const SystemSpecificModelComponent<putils::gl::Mesh> & openGL) noexcept;

		namespace shapes {
			void drawSphere() noexcept;
			void drawQuad() noexcept;
			void drawTexturedQuad() noexcept;
			void drawCone(const glm::vec3 & dir, float radius) noexcept;
			void drawCube() noexcept;
			void drawLine(const glm::vec3 & from, const glm::vec3 & to) noexcept;
		}

		struct BindFramebuffer {
			BindFramebuffer(GLint fbo) noexcept : fbo(fbo) {
				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			}
			~BindFramebuffer() noexcept { glBindFramebuffer(GL_FRAMEBUFFER, prev); }

			GLint fbo;
			GLint prev;
		};

		struct Enable {
			Enable(GLenum feature) noexcept : feature(feature) { glEnable(feature); }
			~Enable() noexcept { glDisable(feature); }

			GLenum feature;
		};

		// Impl
		inline glm::vec3 toVec(const putils::Point3f & p) noexcept {
			return { p.x, p.y, p.z };
		}
	}
}