#pragma once

#include "kengine.hpp"

// putils
#include "Rect.hpp"

// kengine data
#include "data/CameraComponent.hpp"

// kengine functions
#include "functions/AppearsInViewport.hpp"

namespace kengine {
	struct OnScreenComponent;
}

namespace kengine::cameraHelper {
	struct ViewportInfo {
		EntityID camera = INVALID_ID;
		putils::Point2f pixel = { -1.f, -1.f };
		putils::Point2f viewportPercent = { -1.f, -1.f }; // [0,1]
	};
	ViewportInfo getViewportForPixel(EntityID windowID, const putils::Point2ui & pixel) noexcept; 

	bool entityAppearsInViewport(const Entity & entity, const Entity & viewportEntity) noexcept;
    putils::Rect3f convertToScreenPercentage(const putils::Rect3f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept;
    putils::Rect2f convertToScreenPercentage(const putils::Rect2f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept;

	struct Facings {
		putils::Vector3f front;
		putils::Vector3f right;
		putils::Vector3f up;
	};

	Facings getFacings(const CameraComponent & camera) noexcept;
}