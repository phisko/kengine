#include <cmath>
#include <vector>

#include "shaderHelper.hpp"
#include "opengl/Mesh.hpp"

namespace kengine::shaderHelper {
	void drawModel(const SystemSpecificModelComponent<putils::gl::Mesh> & openGL) {
		for (const auto & mesh : openGL.meshes)
			putils::gl::draw(mesh);
	}

	namespace shapes {
		namespace sphere {
#define X .525731112119133606f
#define Z .850650808352039932f
			static GLfloat vdata[12][3] = {
				{-X, 0.f, Z}, {X, 0.f, Z}, {-X, 0.f, -Z}, {X, 0.f, -Z},
				{0.f, Z, X}, {0.f, Z, -X}, {0.f, -Z, X}, {0.f, -Z, -X},
				{Z, X, 0.f}, {-Z, X, 0.f}, {Z, -X, 0.f}, {-Z, -X, 0.f}
			};
			static GLuint tindices[20][3] = {
				{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
				{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
				{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
				{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11} };

			void normalize(GLfloat *a) {
				GLfloat d = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
				a[0] /= d; a[1] /= d; a[2] /= d;
			}

			void drawtri(std::vector<float> & ret, GLfloat *a, GLfloat *b, GLfloat *c, int div, float r) {
				if (div <= 0) {
					ret.push_back(a[0] * r); ret.push_back(a[1] * r); ret.push_back(a[2] * r);
					ret.push_back(b[0] * r); ret.push_back(b[1] * r); ret.push_back(b[2] * r);
					ret.push_back(c[0] * r); ret.push_back(c[1] * r); ret.push_back(c[2] * r);
				}
				else {
					GLfloat ab[3], ac[3], bc[3];
					for (int i = 0; i < 3; i++) {
						ab[i] = (a[i] + b[i]) / 2;
						ac[i] = (a[i] + c[i]) / 2;
						bc[i] = (b[i] + c[i]) / 2;
					}
					normalize(ab); normalize(ac); normalize(bc);
					drawtri(ret, a, ab, ac, div - 1, r);
					drawtri(ret, b, bc, ab, div - 1, r);
					drawtri(ret, c, ac, bc, div - 1, r);
					drawtri(ret, ab, bc, ac, div - 1, r);  //<--Comment this line and sphere looks really cool!
				}
			}
		}

		void drawSphere() {
			static constexpr auto ndiv = 5;
			static const auto radius = .5f;

			static GLuint vao = -1;
			static size_t count = 0;

			if (vao == -1) {
				std::vector<float> vertices;

				for (int i = 0; i < 20; i++)
					sphere::drawtri(vertices, sphere::vdata[sphere::tindices[i][0]], sphere::vdata[sphere::tindices[i][1]], sphere::vdata[sphere::tindices[i][2]], ndiv, radius);

				glGenVertexArrays(1, &vao);
				glBindVertexArray(vao);

				GLuint vbo;
				glGenBuffers(1, &vbo);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

				count = vertices.size();
			}

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)count);
			glBindVertexArray(0);
		}

		void drawQuad() {
			static GLuint vao = 0;
			static GLuint vbo;

			if (vao == 0) {
				const float vertices[] = {
					// positions
					-1.0f,  1.0f, 0.0f,
					-1.0f, -1.0f, 0.0f,
					 1.0f,  1.0f, 0.0f,
					 1.0f, -1.0f, 0.0f,
				};
				// setup plane VAO
				glGenVertexArrays(1, &vao);
				glGenBuffers(1, &vbo);
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
			}

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

		void drawTexturedQuad() {
			static GLuint vao = 0;
			static GLuint vbo;

			if (vao == 0) {
				const float vertices[] = {
					// positions			// texCoords
					-1.0f,  1.0f, 0.0f,		0.f, 1.f,
					-1.0f, -1.0f, 0.0f,		0.f, 0.f,
					 1.0f,  1.0f, 0.0f,		1.f, 1.f,
					 1.0f, -1.0f, 0.0f,		1.f, 0.f,
				};
				// setup plane VAO
				glGenVertexArrays(1, &vao);
				glGenBuffers(1, &vbo);
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			}

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}

		namespace cone {
			glm::vec3 perp(const glm::vec3 &v) {
				auto min = fabs(v.x);
				glm::vec3 cardinalAxis(1.f, 0.f, 0.f);

				if (fabs(v.y) < min) {
					min = fabs(v.y);
					cardinalAxis = { 0.f, 1.f, 0.f };
				}

				if (fabs(v.z) < min) {
					cardinalAxis = { 0.f, 0.f, 1.f };
				}

				return glm::cross(v, cardinalAxis);
			}
		}

		void drawCone(const glm::vec3 & dir, float radius) {
			static constexpr auto radialSlices = 5;
			static const glm::vec3 pointPos{ 0.f, 0.f, 0.f };
			static const auto height = 1.f;

			const auto c = pointPos + (-dir * height);
			const auto e0 = cone::perp(dir);
			const auto e1 = glm::cross(e0, dir);
			const auto angInc = 360.f / radialSlices * 3.14f / 180.f;

			// calculate points around directrix
			std::vector<glm::vec3> pts;
			for (int i = 0; i < radialSlices; ++i) {
				const auto rad = angInc * i;
				const auto p = c + (e0 * cos(rad) + e1 * sin(rad)) * radius;
				pts.push_back(p);
			}

			// draw cone top
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(pointPos.x, pointPos.y, pointPos.z);
			for (int i = 0; i < radialSlices; ++i) {
				glVertex3f(pts[i].x, pts[i].y, pts[i].z);
			}
			glEnd();

			// draw cone bottom
			glBegin(GL_TRIANGLE_FAN);
			glVertex3f(c.x, c.y, c.z);
			for (int i = radialSlices - 1; i >= 0; --i) {
				glVertex3f(pts[i].x, pts[i].y, pts[i].z);
			}
			glEnd();
		}

		void drawCube() {
			static GLuint vao = -1;
			if (vao == -1) {
				static float vertices[] = {
					// positions          
					-.5f,  .5f, -.5f,
					-.5f, -.5f, -.5f,
					 .5f, -.5f, -.5f,
					 .5f, -.5f, -.5f,
					 .5f,  .5f, -.5f,
					-.5f,  .5f, -.5f,

					-.5f, -.5f,  .5f,
					-.5f, -.5f, -.5f,
					-.5f,  .5f, -.5f,
					-.5f,  .5f, -.5f,
					-.5f,  .5f,  .5f,
					-.5f, -.5f,  .5f,

					 .5f, -.5f, -.5f,
					 .5f, -.5f,  .5f,
					 .5f,  .5f,  .5f,
					 .5f,  .5f,  .5f,
					 .5f,  .5f, -.5f,
					 .5f, -.5f, -.5f,

					-.5f, -.5f,  .5f,
					-.5f,  .5f,  .5f,
					 .5f,  .5f,  .5f,
					 .5f,  .5f,  .5f,
					 .5f, -.5f,  .5f,
					-.5f, -.5f,  .5f,

					-.5f,  .5f, -.5f,
					 .5f,  .5f, -.5f,
					 .5f,  .5f,  .5f,
					 .5f,  .5f,  .5f,
					-.5f,  .5f,  .5f,
					-.5f,  .5f, -.5f,

					-.5f, -.5f, -.5f,
					-.5f, -.5f,  .5f,
					 .5f, -.5f, -.5f,
					 .5f, -.5f, -.5f,
					-.5f, -.5f,  .5f,
					 .5f, -.5f,  .5f
				};

				glGenVertexArrays(1, &vao);
				GLuint vbo;
				glGenBuffers(1, &vbo);
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
			}

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		void drawLine(const glm::vec3 & from, const glm::vec3 & to) {
			static GLuint vao = -1;
			static GLuint vbo;

			if (vao == -1) {
				glGenVertexArrays(1, &vao);
				glGenBuffers(1, &vbo);
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
			}

			glBindVertexArray(vao);

			const float vertices[] = {
				from.x, from.y, from.z,
				to.x, to.y, to.z
			};
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);

			glDrawArrays(GL_LINES, 0, 2);
		}
	}
}
