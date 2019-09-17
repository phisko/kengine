#pragma once

#ifndef KENGINE_POLYVOX_CHUNK_SIDE
# define KENGINE_POLYVOX_CHUNK_SIDE 16
#endif

#include <PolyVox/RawVolume.h>

struct PolyVoxObjectComponent : kengine::not_serializable {
	// Indicates that this entity's model should be processed by PolyVoxShader
	pmeta_get_class_name(PolyVoxObjectComponent);
};

struct PolyVoxComponent : kengine::not_serializable {
	struct VertexData {
		float color[3] = { 0.f, 0.f, 0.f };

		pmeta_get_attributes(
			pmeta_reflectible_attribute(&VertexData::color)
		);

		bool operator==(size_t i) const {
			return color[0] == 0.f && color[1] == 0.f && color[2] == 0.f;
		}

		bool operator>(size_t i) const {
			return color[0] != 0.f || color[1] != 0.f || color[2] != 0.f;
		}

		bool operator==(const VertexData & rhs) const {
			return color[0] == rhs.color[0] && color[1] == rhs.color[1] && color[2] == rhs.color[2];
		}
	};

	static constexpr auto CHUNK_SIDE = KENGINE_POLYVOX_CHUNK_SIDE;

	PolyVox::RawVolume<VertexData> volume{ { { 0, 0, 0 }, { CHUNK_SIDE, CHUNK_SIDE, CHUNK_SIDE } } };
	bool changed = true;

	PolyVoxComponent() = default;

	PolyVoxComponent(const PolyVoxComponent & rhs) noexcept {
		*this = rhs;
	}

	PolyVoxComponent & operator=(const PolyVoxComponent & rhs) noexcept {
		const auto & region = volume.getEnclosingRegion();

		for (int z = region.getLowerZ(); z < region.getUpperZ(); ++z)
			for (int y = region.getLowerY(); y < region.getUpperY(); ++y)
				for (int x = region.getLowerX(); x < region.getUpperX(); ++x)
					volume.setVoxel(x, y, z, rhs.volume.getVoxel(x, y, z));

		return *this;
	}

	pmeta_get_class_name(PolyVoxComponent);
};