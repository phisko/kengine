#pragma once

// entt
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

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
		entt::entity camera = entt::null;
		putils::Point2f pixel = { -1.f, -1.f };
		putils::Point2f viewportPercent = { -1.f, -1.f }; // [0,1]
	};
	KENGINE_CORE_EXPORT ViewportInfo getViewportForPixel(entt::handle windowEntity, const putils::Point2ui & pixel) noexcept;

	KENGINE_CORE_EXPORT bool entityAppearsInViewport(const entt::registry & r, entt::entity entity, entt::entity viewportEntity) noexcept;
    KENGINE_CORE_EXPORT putils::Rect3f convertToScreenPercentage(const putils::Rect3f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept;
    KENGINE_CORE_EXPORT putils::Rect2f convertToScreenPercentage(const putils::Rect2f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept;

	struct Facings {
		putils::Vector3f front;
		putils::Vector3f right;
		putils::Vector3f up;
	};

	KENGINE_CORE_EXPORT Facings getFacings(const CameraComponent & camera) noexcept;
}