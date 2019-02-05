#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace shapes {
	void drawSphere();
	void drawQuad();
	void drawCone(const glm::vec3 & dir, float radius);
}