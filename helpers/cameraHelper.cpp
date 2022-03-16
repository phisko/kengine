#include "CameraHelper.hpp"

#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"
#include "data/OnScreenComponent.hpp"
#include "magic_enum.hpp"

namespace kengine::cameraHelper {
	ViewportInfo getViewportForPixel(EntityID windowID, const putils::Point2ui & pixel) noexcept {
		ViewportInfo ret;

		const auto & window = entities[windowID].get<WindowComponent>();

		float highestZ = -std::numeric_limits<float>::max();
		for (const auto & [e, viewport] : entities.with<ViewportComponent>()) {
			if (viewport.window != windowID)
				continue;
			if (highestZ != -std::numeric_limits<float>::max() && highestZ >= viewport.zOrder)
				continue;

			const auto box = cameraHelper::convertToScreenPercentage(viewport.boundingBox, window.size, viewport);

			const auto startX = box.position.x * window.size.x;
			const auto startY = box.position.y * window.size.y;
			const auto sizeX = box.size.x * window.size.x;
			const auto sizeY = box.size.y * window.size.y;

			if (pixel.x < startX || pixel.y < startY ||
				pixel.x >= startX + sizeX ||
				pixel.y >= startY + sizeY)
				continue;

			highestZ = viewport.zOrder;

			ret.camera = e.id;
			ret.pixel = putils::Point2f(pixel) / window.size;
			ret.viewportPercent = (ret.pixel - putils::Point2f(box.position)) / putils::Point2f(box.size);
		}

		return ret;
	}

	bool entityAppearsInViewport(const Entity & e, EntityID viewport) noexcept {
		const auto appearsInViewport = e.tryGet<functions::AppearsInViewport>();
		return !appearsInViewport || appearsInViewport->call(viewport);
	}

    template<size_t N>
    static putils::Rect<float, N> convertToScreenPercentageImpl(const putils::Rect<float, N> & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept {
        switch (comp.coordinateType) {
            case OnScreenComponent::CoordinateType::Pixels: {
                putils::Rect<float, N> ret = rect;
                ret.position.x /= screenSize.x;
                ret.position.y /= screenSize.y;
                ret.size.x /= screenSize.x;
                ret.size.y /= screenSize.y;
                return ret;
            }
            case OnScreenComponent::CoordinateType::ScreenPercentage:
                return rect;
            default:
                static_assert(putils::magic_enum::enum_count<OnScreenComponent::CoordinateType>() == 2);
                return rect;
        }
    }

	putils::Rect3f convertToScreenPercentage(const putils::Rect3f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept {
        return convertToScreenPercentageImpl(rect, screenSize, comp);
	}

    putils::Rect2f convertToScreenPercentage(const putils::Rect2f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept {
        return convertToScreenPercentageImpl(rect, screenSize, comp);
    }

	Facings getFacings(const CameraComponent & camera) noexcept {
		struct impl {
			static Facings getFacings(const CameraComponent & camera) noexcept {
				Facings ret;

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

			static void rotate(putils::Point3f & dst, const putils::Point3f & axis, float angle) noexcept {
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

			static void axisToQuaternion(float * quat, const putils::Point3f & axis, float angle) noexcept {
				angle = angle * 0.5f;
				const float sinus = std::sin(angle);
				quat[0] = sinus * axis[0];
				quat[1] = sinus * axis[1];
				quat[2] = sinus * axis[2];
				quat[3] = std::cos(angle);
			}

			static void quaternionToMatrix(float * mat, const float * qs) noexcept {
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

		return impl::getFacings(camera);
	}
}