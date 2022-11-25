#include "camera_helper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// kengine data
#include "kengine/data/viewport.hpp"
#include "kengine/data/window.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::camera_helper {
	viewport_info get_viewport_for_pixel(entt::handle window_entity, const putils::point2ui & pixel) noexcept {
		KENGINE_PROFILING_SCOPE;

		viewport_info ret;

		const auto & window = window_entity.get<data::window>();

		float highest_z = -std::numeric_limits<float>::max();
		for (const auto & [e, viewport] : window_entity.registry()->view<data::viewport>().each()) {
			if (viewport.window != window_entity)
				continue;
			if (highest_z != -std::numeric_limits<float>::max() && highest_z >= viewport.z_order)
				continue;

			const auto box = camera_helper::convert_to_screen_percentage(viewport.bounding_box, window.size, viewport);

			const auto start_x = box.position.x * window.size.x;
			const auto start_y = box.position.y * window.size.y;
			const auto size_x = box.size.x * window.size.x;
			const auto size_y = box.size.y * window.size.y;

			if (pixel.x < start_x || pixel.y < start_y ||
				pixel.x >= start_x + size_x ||
				pixel.y >= start_y + size_y)
				continue;

			highest_z = viewport.z_order;

			ret.camera = e;
			ret.pixel = putils::point2f(pixel) / window.size;
			ret.viewport_percent = (ret.pixel - putils::point2f(box.position)) / putils::point2f(box.size);
		}

		return ret;
	}

	bool entity_appears_in_viewport(const entt::registry & r, entt::entity entity, entt::entity viewport_entity) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto wants_to_appear = [&](entt::entity lhs, entt::entity rhs) noexcept {
			if (const auto appears_in_viewport = r.try_get<functions::appears_in_viewport>(lhs))
				if (!appears_in_viewport->call(rhs))
					return false;
			return true;
		};

		return wants_to_appear(entity, viewport_entity) && wants_to_appear(viewport_entity, entity);
	}

    template<size_t N>
    static putils::rect<float, N> convert_to_screen_percentage_impl(const putils::rect<float, N> & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept {
		KENGINE_PROFILING_SCOPE;

        switch (comp.coordinates) {
            case data::on_screen::coordinate_type::pixels: {
                putils::rect<float, N> ret = rect;
                ret.position.x /= screen_size.x;
                ret.position.y /= screen_size.y;
                ret.size.x /= screen_size.x;
                ret.size.y /= screen_size.y;
                return ret;
            }
            case data::on_screen::coordinate_type::screen_percentage:
                return rect;
            default:
                static_assert(magic_enum::enum_count<data::on_screen::coordinate_type>() == 2);
                return rect;
        }
    }

	putils::rect3f convert_to_screen_percentage(const putils::rect3f & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept {
        return convert_to_screen_percentage_impl(rect, screen_size, comp);
	}

    putils::rect2f convert_to_screen_percentage(const putils::rect2f & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept {
        return convert_to_screen_percentage_impl(rect, screen_size, comp);
    }

	facings get_facings(const data::camera & camera) noexcept {
		KENGINE_PROFILING_SCOPE;

		struct impl {
			static facings get_facings(const data::camera & camera) noexcept {
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
		};

		return impl::get_facings(camera);
	}
}