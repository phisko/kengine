#pragma once

namespace kengine::detail {
	static void axisToQuaternion(float * quat, const glm::vec3 & axis, float angle) {
		angle = angle * 0.5f;
		const float sinus = std::sin(angle);
		quat[0] = sinus * axis[0];
		quat[1] = sinus * axis[1];
		quat[2] = sinus * axis[2];
		quat[3] = std::cos(angle);
	}

	inline void quaternionToMatrix(float * mat, const float *qs) {
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

		xs = q[0] * s;   ys = q[1] * s;  zs = q[2] * s;
		wx = q[3] * xs;  wy = q[3] * ys; wz = q[3] * zs;
		xx = q[0] * xs;  xy = q[0] * ys; xz = q[0] * zs;
		yy = q[1] * ys;  yz = q[1] * zs; zz = q[2] * zs;

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

	static void rotate(glm::vec3 & dst, const glm::vec3 & axis, float angle) {
		float quat[4];
		axisToQuaternion(quat, axis, angle);
		float mat[9];
		quaternionToMatrix(mat, quat);

		float x, y, z;
		x = dst[0] * mat[0] + dst[1] * mat[3] + dst[2] * mat[6];
		y = dst[0] * mat[1] + dst[1] * mat[4] + dst[2] * mat[7];
		z = dst[0] * mat[2] + dst[1] * mat[5] + dst[2] * mat[8];
		dst[0] = x;
		dst[1] = y;
		dst[2] = z;
	}
}