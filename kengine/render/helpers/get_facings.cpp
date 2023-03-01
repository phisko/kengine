#include "get_facings.hpp"

// kengine
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::render {
	facings get_facings(const camera & camera) noexcept {
		KENGINE_PROFILING_SCOPE;

		struct impl {
			static facings get_facings(const render::camera & camera) noexcept {
				facings ret;

				ret.front = {
					std::sin(camera.yaw) * std::cos(camera.pitch),
					std::sin(camera.pitch),
					std::cos(camera.yaw) * std::cos(camera.pitch)
				};
				putils::normalize(ret.front);

				ret.right = putils::cross(ret.front, { 0.f, 1.f, 0.f });
				putils::normalize(ret.right);

				ret.up = putils::cross(ret.right, ret.front);
				putils::normalize(ret.up);

				rotate(ret.up, ret.front, camera.roll);
				return ret;
			}

			static void rotate(putils::point3f & dst, const putils::point3f & axis, float angle) noexcept {
				float quat[4];
				axis_to_quaternion(quat, axis, angle);
				float mat[9];
				quaternion_to_matrix(mat, quat);

				float x, y, z;
				x = dst[0] * mat[0] + dst[1] * mat[3] + dst[2] * mat[6];
				y = dst[0] * mat[1] + dst[1] * mat[4] + dst[2] * mat[7];
				z = dst[0] * mat[2] + dst[1] * mat[5] + dst[2] * mat[8];
				dst[0] = x;
				dst[1] = y;
				dst[2] = z;
			}

			static void axis_to_quaternion(float * quat, const putils::point3f & axis, float angle) noexcept {
				angle = angle * 0.5f;
				const float sinus = std::sin(angle);
				quat[0] = sinus * axis[0];
				quat[1] = sinus * axis[1];
				quat[2] = sinus * axis[2];
				quat[3] = std::cos(angle);
			}

			static void quaternion_to_matrix(float * mat, const float * qs) noexcept {
				float q[4];
				float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz, den;
				q[0] = -qs[0];
				q[1] = -qs[1];
				q[2] = -qs[2];
				q[3] = qs[3];

				den = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);

				if (den == 0.0f)
					s = 1.0f;
				else
					s = 2.0f / den;

				xs = q[0] * s;
				ys = q[1] * s;
				zs = q[2] * s;
				wx = q[3] * xs;
				wy = q[3] * ys;
				wz = q[3] * zs;
				xx = q[0] * xs;
				xy = q[0] * ys;
				xz = q[0] * zs;
				yy = q[1] * ys;
				yz = q[1] * zs;
				zz = q[2] * zs;

				mat[0] = 1.0f - (yy + zz);
				mat[3] = xy - wz;
				mat[6] = xz + wy;

				mat[1] = xy + wz;
				mat[4] = 1.0f - (xx + zz);
				mat[7] = yz - wx;

				mat[2] = xz - wy;
				mat[5] = yz + wx;
				mat[8] = 1.0f - (xx + yy);
			}
		};

		return impl::get_facings(camera);
	}
}