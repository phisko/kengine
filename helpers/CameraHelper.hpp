#pragma once

#include "Entity.hpp"
#include "EntityManager.hpp"
#include "Point.hpp"

namespace kengine::CameraHelper {
	struct ViewportInfo {
		Entity::ID camera = Entity::INVALID_ID;
		putils::Point2f pixel = { -1.f, -1.f }; // [0,1]
	};

	ViewportInfo getViewportForPixel(EntityManager & em, Entity::ID windowID, const putils::Point2ui & pixel); 
}