#pragma once

#ifndef KENGINE_NAVMESH_MAX_PATH_LENGTH
# define KENGINE_NAVMESH_MAX_PATH_LENGTH 128
#endif

#ifndef KENGINE_NAVMESH_FUNC_SIZE
# define KENGINE_NAVMESH_FUNC_SIZE 64
#endif

#include "reflection.hpp"
#include "angle.hpp"
#include "Point.hpp"
#include "vector.hpp"
#include "function.hpp"

namespace kengine {
	struct RebuildNavMeshComponent { // Indicates that the navmesh should be rebuilt
		putils_reflection_class_name(RebuildNavMeshComponent);
	};

	struct NavMeshComponent {
		using Path = putils::vector<putils::Point3f, KENGINE_NAVMESH_MAX_PATH_LENGTH>;
		using GetPathFunc = putils::function<Path(const Entity & e, const putils::Point3f & start, const putils::Point3f & end), KENGINE_NAVMESH_FUNC_SIZE>;
		// `e` is the entity instantiating the `model Entity` this component is attached to

		GetPathFunc getPath = nullptr; // Filled by System that generates navmesh
		size_t concernedMesh = 0; // Index into ModelDataComponent::meshes, pointing to the mesh for which to generate the navmesh
		float cellSize = .25f;
		float cellHeight = .25f;
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
			putils_reflection_attribute(&NavMeshComponent::getPath),
			putils_reflection_attribute(&NavMeshComponent::concernedMesh),
			putils_reflection_attribute(&NavMeshComponent::cellSize),
			putils_reflection_attribute(&NavMeshComponent::cellHeight),
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