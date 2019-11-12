#include "CameraHelper.hpp"

#include "components/ViewportComponent.hpp"

namespace kengine::CameraHelper {
	ViewportInfo getViewportForPixel(EntityManager & em, const putils::Point2ui & pixel, const putils::Point2ui & screenSize) {
		ViewportInfo ret;

		for (const auto & [e, viewport] : em.getEntities<ViewportComponent>()) {
			const auto startX = viewport.boundingBox.position.x * screenSize.x;
			const auto startY = viewport.boundingBox.position.y * screenSize.y;
			const auto sizeX = viewport.boundingBox.size.x * screenSize.x;
			const auto sizeY = viewport.boundingBox.size.y * screenSize.y;

			if (pixel.x < startX || pixel.y < startY ||
				pixel.x >= startX + sizeX ||
				pixel.y >= startY + sizeY)
				continue;

			const auto pixelSreenPercent = putils::Point2f(pixel) / screenSize;
			const auto pixelViewportPercent = (pixelSreenPercent - viewport.boundingBox.position) / viewport.boundingBox.size;

			ret.camera = e.id;
			ret.pixel = pixelViewportPercent;
			return ret;
		}

		return ret;
	}
}