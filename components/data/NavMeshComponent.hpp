#pragma once

#include "reflection.hpp"
#include "angle.hpp"

namespace kengine {
	struct NavMeshComponent {
		bool mustRebuild = true;
		float cellSize = .25f;
		float walkableSlope = putils::pi / 4.f;
		float characterHeight = 1.f;
		float characterClimb = .75f;
		float characterRadius = .5f;
		int maxEdgeLength = 80;
		float maxSimplificationError = 1.1f;
		float minRegionArea = 9.f;
		float mergeRegionArea = 25.f;
		int vertsPerPoly = 6;
		float detailSampleDist = 75.f;
		float detailSampleMaxError = 20.f;
		int queryMaxSearchNodes = 65535;

		putils_reflection_class_name(NavMeshComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&NavMeshComponent::mustRebuild),
			putils_reflection_attribute(&NavMeshComponent::cellSize),
			putils_reflection_attribute(&NavMeshComponent::walkableSlope),
			putils_reflection_attribute(&NavMeshComponent::characterHeight),
			putils_reflection_attribute(&NavMeshComponent::characterClimb),
			putils_reflection_attribute(&NavMeshComponent::characterRadius),
			putils_reflection_attribute(&NavMeshComponent::maxEdgeLength),
			putils_reflection_attribute(&NavMeshComponent::maxSimplificationError),
			putils_reflection_attribute(&NavMeshComponent::minRegionArea),
			putils_reflection_attribute(&NavMeshComponent::mergeRegionArea),
			putils_reflection_attribute(&NavMeshComponent::vertsPerPoly),
			putils_reflection_attribute(&NavMeshComponent::detailSampleDist),
			putils_reflection_attribute(&NavMeshComponent::detailSampleMaxError),
			putils_reflection_attribute(&NavMeshComponent::queryMaxSearchNodes)
		);
	};
}