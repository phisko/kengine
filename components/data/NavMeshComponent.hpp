#pragma once

#ifndef KENGINE_NAVMESH_MAX_PATH_LENGTH
# define KENGINE_NAVMESH_MAX_PATH_LENGTH 128
#endif

#ifndef KENGINE_NAVMESH_FUNC_SIZE
# define KENGINE_NAVMESH_FUNC_SIZE 64
#endif

// reflection
#include "reflection.hpp"

// putils
#include "angle.hpp"
#include "Point.hpp"
#include "vector.hpp"

// kengine functions
#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	struct NavMeshComponent {
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
	};

	namespace functions {
		namespace GetPathImpl {
			static constexpr char PathName[] = "NavMeshComponentPath";
			using Path = putils::vector<putils::Point3f, KENGINE_NAVMESH_MAX_PATH_LENGTH, PathName>;
		}

#ifdef __GNUC__
// Ignore "'...' has a base '...' whose type uses the anonymous namespace" warnings
// Haven't found any type here that uses the anonymous namespace, not sure where this is coming from
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wsubobject-linkage"
#endif

		struct GetPath : BaseFunction<
			GetPathImpl::Path (const Entity & environment, const putils::Point3f & start, const putils::Point3f & end)
			// `environment` is the entity instantiating the `model Entity` this component is attached to
		> {};

#ifdef __GNU_C__
#	pragma GCC diagnostic pop
#endif

	}
}

#define refltype kengine::NavMeshComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(concernedMesh),
		putils_reflection_attribute(cellSize),
		putils_reflection_attribute(cellHeight),
		putils_reflection_attribute(walkableSlope),
		putils_reflection_attribute(characterHeight),
		putils_reflection_attribute(characterClimb),
		putils_reflection_attribute(characterRadius),
		putils_reflection_attribute(maxEdgeLength),
		putils_reflection_attribute(maxSimplificationError),
		putils_reflection_attribute(minRegionArea),
		putils_reflection_attribute(mergeRegionArea),
		putils_reflection_attribute(vertsPerPoly),
		putils_reflection_attribute(detailSampleDist),
		putils_reflection_attribute(detailSampleMaxError),
		putils_reflection_attribute(queryMaxSearchNodes)
	);
};
#undef refltype

#define refltype kengine::functions::GetPath
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
	putils_reflection_used_types(
		putils_reflection_type(kengine::functions::GetPathImpl::Path)
	);
};
#undef refltype