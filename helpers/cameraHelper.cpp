#include "CameraHelper.hpp"

#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"
#include "data/OnScreenComponent.hpp"
#include "magic_enum.hpp"

namespace kengine::cameraHelper {
	ViewportInfo getViewportForPixel(EntityManager & em, Entity::ID windowID, const putils::Point2ui & pixel) {
		ViewportInfo ret;

		const auto & window = em.getEntity(windowID).get<WindowComponent>();

		float highestZ = -FLT_MAX;
		for (const auto & [e, viewport] : em.getEntities<ViewportComponent>()) {
			if (viewport.window != windowID)
				continue;
			if (highestZ != -FLT_MAX && highestZ >= viewport.zOrder)
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

	putils::Rect3f convertToScreenPercentage(const putils::Rect3f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) {
		switch (comp.coordinateType) {
		case OnScreenComponent::CoordinateType::Pixels: {
			putils::Rect3f ret = rect;
			ret.position.x /= screenSize.x;
			ret.position.y /= screenSize.y;
			ret.size.x /= screenSize.x;
			ret.size.y /= screenSize.y;
			return ret;
		}
		case OnScreenComponent::CoordinateType::ScreenPercentage:
			return rect;
		default:
			assert(!"Unsupported coordinate type");
			static_assert(putils::magic_enum::enum_count<OnScreenComponent::CoordinateType>() == 2);
			return rect;
		}
	}
}