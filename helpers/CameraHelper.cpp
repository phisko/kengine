#include "CameraHelper.hpp"

#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"

namespace kengine::CameraHelper {
	ViewportInfo getViewportForPixel(EntityManager & em, Entity::ID windowID, const putils::Point2ui & pixel) {
		ViewportInfo ret;

		const auto & window = em.getEntity(windowID).get<WindowComponent>();

		float highestZ = -FLT_MAX;
		for (const auto & [e, viewport] : em.getEntities<ViewportComponent>()) {
			if (viewport.window != windowID)
				continue;
			if (highestZ != -FLT_MAX && highestZ >= viewport.zOrder)
				continue;

			const auto startX = viewport.boundingBox.position.x * window.size.x;
			const auto startY = viewport.boundingBox.position.y * window.size.y;
			const auto sizeX = viewport.boundingBox.size.x * window.size.x;
			const auto sizeY = viewport.boundingBox.size.y * window.size.y;

			if (pixel.x < startX || pixel.y < startY ||
				pixel.x >= startX + sizeX ||
				pixel.y >= startY + sizeY)
				continue;

			highestZ = viewport.zOrder;

			const auto pixelSreenPercent = putils::Point2f(pixel) / window.size;
			const auto pixelViewportPercent = (pixelSreenPercent - viewport.boundingBox.position) / viewport.boundingBox.size;

			ret.camera = e.id;
			ret.pixel = pixelViewportPercent;
		}

		return ret;
	}
}