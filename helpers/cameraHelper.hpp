#pragma once

#include "Entity.hpp"
#include "EntityManager.hpp"

#include "data/CameraComponent.hpp"
#include "functions/AppearsInViewport.hpp"

namespace kengine {
	struct OnScreenComponent;
}

namespace kengine::cameraHelper {
	struct ViewportInfo {
		Entity::ID camera = Entity::INVALID_ID;
		putils::Point2f pixel = { -1.f, -1.f };
		putils::Point2f viewportPercent = { -1.f, -1.f }; // [0,1]
	};
	ViewportInfo getViewportForPixel(EntityManager & em, Entity::ID windowID, const putils::Point2ui & pixel); 

	inline bool entityAppearsInViewport(const Entity & e, Entity::ID viewport);
	putils::Rect3f convertToScreenPercentage(const putils::Rect3f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp);

	struct Facings {
		putils::Vector3f front;
		putils::Vector3f right;
		putils::Vector3f up;
	};

	Facings getFacings(const CameraComponent & camera);

	// Impl

	inline bool entityAppearsInViewport(const Entity & e, Entity::ID viewport) {
		const auto appearsInViewport = e.tryGet<functions::AppearsInViewport>();
		return !appearsInViewport || appearsInViewport->call(viewport);
	}
}