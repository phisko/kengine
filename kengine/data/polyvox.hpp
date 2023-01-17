#pragma once

#ifdef KENGINE_POLYVOX

#ifndef KENGINE_POLYVOX_CHUNK_SIDE
#define KENGINE_POLYVOX_CHUNK_SIDE 16
#endif

// polyvox
#include <PolyVox/RawVolume.h>

namespace kengine::data {
	//! putils reflect none
	struct polyvox {
		//! putils reflect all
		struct vertex_data {
			float color[3] = { 0.f, 0.f, 0.f };

			bool operator==(size_t i) const {
				return color[0] == 0.f && color[1] == 0.f && color[2] == 0.f;
			}

			bool operator>(size_t i) const {
				return color[0] != 0.f || color[1] != 0.f || color[2] != 0.f;
			}

			bool operator==(const vertex_data & rhs) const {
				return color[0] == rhs.color[0] && color[1] == rhs.color[1] && color[2] == rhs.color[2];
			}
		};

		static constexpr auto CHUNK_SIDE = KENGINE_POLYVOX_CHUNK_SIDE;

		PolyVox::RawVolume<vertex_data> volume{ { { 0, 0, 0 }, { CHUNK_SIDE, CHUNK_SIDE, CHUNK_SIDE } } };
		bool changed = true;

		polyvox() = default;

		polyvox(const polyvox & rhs) noexcept {
			*this = rhs;
		}

		polyvox & operator=(const polyvox & rhs) noexcept {
			const auto & region = volume.getEnclosingRegion();

			for (int z = region.getLowerZ(); z < region.getUpperZ(); ++z)
				for (int y = region.getLowerY(); y < region.getUpperY(); ++y)
					for (int x = region.getLowerX(); x < region.getUpperX(); ++x)
						volume.setVoxel(x, y, z, rhs.volume.getVoxel(x, y, z));

			return *this;
		}
	};
}

#include "polyvox.reflection.hpp"

#endif