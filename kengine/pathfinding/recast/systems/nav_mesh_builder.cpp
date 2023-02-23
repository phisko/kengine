// stl
#include <filesystem>
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// recast
#include <Recast.h>
#include <DetourNavMeshBuilder.h>

// putils
#include "putils/lengthof.hpp"
#include "putils/on_scope_exit.hpp"
#include "putils/thread_name.hpp"
#include "putils/with.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine async
#include "kengine/async/helpers/start_task.hpp"
#include "kengine/async/helpers/process_results.hpp"

// kengine glm
#include "kengine/glm/helpers/convert_to_referencial.hpp"
#include "kengine/glm/helpers/get_model_matrix.hpp"

// kengine instance
#include "kengine/render/data/asset.hpp"
#include "kengine/render/data/model_data.hpp"

// kengine pathfinding
#include "kengine/pathfinding/data/nav_mesh.hpp"

// kengine pathfinding/recast
#include "common.hpp"
#include "kengine/pathfinding/recast/data/recast_nav_mesh.hpp"

namespace kengine::systems::recast_impl {
	struct build_recast_component {
		using height_field_ptr = unique_ptr<rcHeightfield, rcFreeHeightField>;
		using compact_height_field_ptr = unique_ptr<rcCompactHeightfield, rcFreeCompactHeightfield>;
		using contour_set_ptr = unique_ptr<rcContourSet, rcFreeContourSet>;
		using poly_mesh_ptr = unique_ptr<rcPolyMesh, rcFreePolyMesh>;
		using poly_mesh_detail_ptr = unique_ptr<rcPolyMeshDetail, rcFreePolyMeshDetail>;

		struct nav_mesh_data {
			unsigned char * data = nullptr;
			int size = 0;
			float area_size = 0.f;
		};

		static std::optional<data::recast_nav_mesh> create_recast_mesh(const char * file, entt::handle e, const data::nav_mesh & nav_mesh, const data::model_data & model_data) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), verbose, "recast", "Building navmesh for %s", file);

			const auto & r = *e.registry();

			data::recast_nav_mesh result;

			const putils::string<4096> binary_file("%s.nav", file);
			result.data = load_binary_file(binary_file.c_str(), nav_mesh);
			if (result.data.data) {
				kengine_log(r, verbose, "recast", "Found binary file");
			}
			else {
				kengine_logf(r, verbose, "recast", "Found no binary file for %s, creating nav mesh data", file);
				result.data = create_nav_mesh_data(r, nav_mesh, model_data, model_data.meshes[nav_mesh.concerned_mesh]);
				if (result.data.data == nullptr)
					return std::nullopt;
				save_binary_file(r, binary_file.c_str(), result.data, nav_mesh);
			}

			result.nav_mesh = create_nav_mesh(r, result.data);
			if (result.nav_mesh == nullptr)
				return std::nullopt;

			result.nav_mesh_query = create_nav_mesh_query(r, nav_mesh, *result.nav_mesh);
			if (result.nav_mesh_query == nullptr)
				return std::nullopt;

			return result;
		}

		static data::nav_mesh_data load_binary_file(const char * binary_file, const data::nav_mesh & nav_mesh) noexcept {
			KENGINE_PROFILING_SCOPE;

			data::nav_mesh_data data;

			std::ifstream f(binary_file, std::ifstream::binary);
			if (!f)
				return data;

			data::nav_mesh header;
			f.read((char *)&header, sizeof(header));
			if (std::memcmp(&header, &nav_mesh, sizeof(header)))
				return data; // Different parameters

			f.read((char *)&data.size, sizeof(data.size));
			data.data.reset(dtAlloc(data.size, dtAllocHint::DT_ALLOC_PERM));
			f.read((char *)data.data.get(), data.size);

			return data;
		}

		static void save_binary_file(const entt::registry & r, const char * binary_file, const data::nav_mesh_data & data, const data::nav_mesh & nav_mesh) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, "recast", "Saving binary file %s", binary_file);

			std::ofstream f(binary_file, std::ofstream::trunc | std::ofstream::binary);
			f.write((const char *)&nav_mesh, sizeof(nav_mesh));
			f.write((const char *)&data.size, sizeof(data.size));
			f.write((const char *)data.data.get(), data.size);
		}

		static data::nav_mesh_data create_nav_mesh_data(const entt::registry & r, const data::nav_mesh & nav_mesh, const data::model_data & model_data, const data::model_data::mesh & mesh_data) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "recast", "Creating navmesh data");

			data::nav_mesh_data ret;

			const auto vertices = get_vertices(r, model_data, mesh_data);

			const auto cfg = get_config(r, nav_mesh, mesh_data, vertices.get());
			if (cfg.width == 0 || cfg.height == 0) {
				kengine_assert_failed(r, "[Recast] Mesh was 0 height or width?");
				return ret;
			}

			rcContext ctx;

			const auto height_field = create_height_field(r, ctx, cfg, mesh_data, vertices.get());
			if (height_field == nullptr)
				return ret;

			rcFilterLowHangingWalkableObstacles(&ctx, cfg.walkableClimb, *height_field);
			rcFilterLedgeSpans(&ctx, cfg.walkableHeight, cfg.walkableClimb, *height_field);
			rcFilterWalkableLowHeightSpans(&ctx, cfg.walkableHeight, *height_field);

			const auto compact_height_field = create_compact_height_field(r, ctx, cfg, *height_field);
			if (compact_height_field == nullptr)
				return ret;

			const auto contour_set = create_contour_set(r, ctx, cfg, *compact_height_field);
			if (contour_set == nullptr)
				return ret;

			const auto poly_mesh = create_poly_mesh(r, ctx, cfg, *contour_set);
			if (poly_mesh == nullptr)
				return ret;

			const auto poly_mesh_detail = create_poly_mesh_detail(r, ctx, cfg, *poly_mesh, *compact_height_field);
			if (poly_mesh_detail == nullptr)
				return ret;

			for (int i = 0; i < poly_mesh->npolys; ++i)
				if (poly_mesh->areas[i] == RC_WALKABLE_AREA)
					poly_mesh->flags[i] = flags::walk;

			dtNavMeshCreateParams params;
			memset(&params, 0, sizeof(params));
			{
				putils_with(*poly_mesh);
				{
					params.verts = _.verts;
					params.vertCount = _.nverts;
					params.polys = _.polys;
					params.polyAreas = _.areas;
					params.polyFlags = _.flags;
					params.polyCount = _.npolys;
					params.nvp = _.nvp;
				}
			}

			{
				putils_with(*poly_mesh_detail);
				{
					params.detailMeshes = _.meshes;
					params.detailVerts = _.verts;
					params.detailVertsCount = _.nverts;
					params.detailTris = _.tris;
					params.detailTriCount = _.ntris;
				}
			}

			params.walkableHeight = (float)cfg.walkableHeight;
			params.walkableClimb = (float)cfg.walkableClimb;
			params.walkableRadius = (float)cfg.walkableRadius;
			rcVcopy(params.bmin, cfg.bmin);
			rcVcopy(params.bmax, cfg.bmax);
			params.cs = cfg.cs;
			params.ch = cfg.ch;

			unsigned char * data = nullptr;
			if (!dtCreateNavMeshData(&params, &data, &ret.size))
				kengine_assert_failed(r, "[Recast] Failed to create Detour navmesh data");
			ret.data.reset(data);

			return ret;
		}

		static std::unique_ptr<float[]> get_vertices(const entt::registry & r, const data::model_data & model_data, const data::model_data::mesh & mesh_data) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Getting vertices");

			const auto position_offset = get_vertex_position_offset(r, model_data);
			if (position_offset == std::nullopt)
				return nullptr;

			auto vertices = std::unique_ptr<float[]>(new float[mesh_data.vertices.nb_elements * 3]);

			for (size_t vertex = 0; vertex < mesh_data.vertices.nb_elements; ++vertex) {
				const auto pos = get_vertex_position(mesh_data.vertices.data, vertex, model_data.vertex_size, *position_offset);
				for (size_t i = 0; i < 3; ++i)
					vertices[vertex * 3 + i] = pos[i];
			}

			return vertices;
		}

		static std::optional<std::ptrdiff_t> get_vertex_position_offset(const entt::registry & r, const data::model_data & model_data) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Getting vertex position offset");

			static const std::string_view potential_names[] = { "pos", "position" };

			for (const auto name : potential_names)
				for (const auto & attribute : model_data.vertex_attributes)
					if (attribute.name == name) {
						kengine_logf(r, very_verbose, "recast", "Found vertex position offset [%u] (named %s)", attribute.offset, attribute.name);
						return attribute.offset;
					}

			kengine_assert_failed(r, "[Recast] Could not find vertex position");
			return std::nullopt;
		}

		static const float * get_vertex_position(const void * vertices, size_t index, size_t vertex_size, std::ptrdiff_t position_offset) noexcept {
			const auto vertex = (const char *)vertices + index * vertex_size;
			return (const float *)(vertex + position_offset);
		}

		static rcConfig get_config(const entt::registry & r, const data::nav_mesh & nav_mesh, const data::model_data::mesh & mesh_data, const float * vertices) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Getting rcConfig");

			rcConfig cfg;
			memset(&cfg, 0, sizeof(cfg));

			{
				putils_with(nav_mesh);
				{
					cfg.cs = _.cell_size;
					kengine_assert(r, cfg.cs > 0);

					cfg.ch = _.cell_height;
					kengine_assert(r, cfg.ch > 0);

					cfg.walkableSlopeAngle = putils::to_degrees(_.walkable_slope);
					kengine_assert(r, cfg.walkableSlopeAngle > 0.f && cfg.walkableSlopeAngle <= 90.f);

					cfg.walkableHeight = (int)ceilf(_.character_height / _.cell_height);
					kengine_assert(r, cfg.walkableHeight >= 3);

					cfg.walkableClimb = (int)floorf(_.character_climb / _.cell_height);
					kengine_assert(r, cfg.walkableClimb >= 0);

					cfg.walkableRadius = (int)ceilf(_.character_radius / _.cell_size);
					kengine_assert(r, cfg.walkableRadius >= 0);

					cfg.maxEdgeLen = (int)(_.max_edge_length / _.cell_size);
					kengine_assert(r, cfg.maxEdgeLen >= 0);

					cfg.maxSimplificationError = _.max_simplification_error;
					kengine_assert(r, cfg.maxSimplificationError >= 0);

					cfg.minRegionArea = (int)rcSqr(_.min_region_area);
					kengine_assert(r, cfg.minRegionArea >= 0);

					cfg.mergeRegionArea = (int)rcSqr(_.merge_region_area);
					kengine_assert(r, cfg.mergeRegionArea >= 0);

					cfg.maxVertsPerPoly = _.verts_per_poly;
					kengine_assert(r, cfg.maxVertsPerPoly >= 3);

					cfg.detailSampleDist = _.detail_sample_dist;
					kengine_assert(r, cfg.detailSampleDist == 0.f || cfg.detailSampleDist >= .9f);

					cfg.detailSampleMaxError = _.detail_sample_max_error;
					kengine_assert(r, cfg.detailSampleMaxError >= 0.f);
				}
			}

			rcCalcBounds(vertices, (int)mesh_data.vertices.nb_elements, cfg.bmin, cfg.bmax);
			rcCalcGridSize(cfg.bmin, cfg.bmax, cfg.cs, &cfg.width, &cfg.height);

			return cfg;
		}

		static height_field_ptr create_height_field(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, const kengine::data::model_data::mesh & mesh_data, const float * vertices) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating height field");

			height_field_ptr height_field{ rcAllocHeightfield() };

			if (height_field == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate height field");
				return nullptr;
			}

			if (!rcCreateHeightfield(&ctx, *height_field, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)) {
				kengine_assert_failed(r, "[Recast] Failed to create height field");
				return nullptr;
			}

			const auto nb_triangles = mesh_data.indices.nb_elements / 3; // I think?
			const auto triangle_areas = new unsigned char[nb_triangles];
			memset(triangle_areas, 0, nb_triangles);

			int * indices = (int *)mesh_data.indices.data;
			bool must_delete_indices = false;
			if (mesh_data.index_type == putils::meta::type<unsigned int>::index) {
				indices = new int[mesh_data.indices.nb_elements];
				must_delete_indices = true;
				const auto unsigned_indices = (const unsigned int *)mesh_data.indices.data;
				for (int i = 0; i < mesh_data.indices.nb_elements; ++i)
					indices[i] = (int)unsigned_indices[i];
			}

			rcMarkWalkableTriangles(
				&ctx, cfg.walkableSlopeAngle,
				vertices, (int)mesh_data.vertices.nb_elements,
				indices, (int)nb_triangles,
				triangle_areas
			);

			if (!rcRasterizeTriangles(&ctx, vertices, (int)mesh_data.vertices.nb_elements, indices, triangle_areas, (int)nb_triangles, *height_field, cfg.walkableClimb)) {
				kengine_assert_failed(r, "[Recast] Failed to rasterize triangles");
				delete[] triangle_areas;
				return nullptr;
			}

			if (must_delete_indices)
				delete[] indices;

			delete[] triangle_areas;

			return height_field;
		}

		static compact_height_field_ptr create_compact_height_field(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, rcHeightfield & height_field) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating compact height field");

			compact_height_field_ptr compact_height_field{ rcAllocCompactHeightfield() };

			if (compact_height_field == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate compact height field");
				return nullptr;
			}

			if (!rcBuildCompactHeightfield(&ctx, cfg.walkableHeight, cfg.walkableClimb, height_field, *compact_height_field)) {
				kengine_assert_failed(r, "[Recast] Failed to build compact height field");
				return nullptr;
			}

			if (!rcErodeWalkableArea(&ctx, cfg.walkableRadius, *compact_height_field)) {
				kengine_assert_failed(r, "[Recast] Failed to erode walkable area");
				return nullptr;
			}

			// Classic recast positiong. For others, see https://github.com/recastnavigation/recastnavigation/blob/master/RecastDemo/Source/Sample_SoloMesh.cpp
			if (!rcBuildDistanceField(&ctx, *compact_height_field)) {
				kengine_assert_failed(r, "[Recast] Failed to build distance field");
				return nullptr;
			}

			if (!rcBuildRegions(&ctx, *compact_height_field, 0, cfg.minRegionArea, cfg.mergeRegionArea)) {
				kengine_assert_failed(r, "[Recast] Failed to build regions");
				return nullptr;
			}

			return compact_height_field;
		}

		static contour_set_ptr create_contour_set(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, rcCompactHeightfield & chf) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating contour set");

			contour_set_ptr contour_set{ rcAllocContourSet() };

			if (contour_set == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate contour set");
				return nullptr;
			}

			if (!rcBuildContours(&ctx, chf, cfg.maxSimplificationError, cfg.maxEdgeLen, *contour_set)) {
				kengine_assert_failed(r, "[Recast] Failed to build contours");
				return nullptr;
			}

			return contour_set;
		}

		static poly_mesh_ptr create_poly_mesh(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, rcContourSet & contour_set) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating poly mesh");

			poly_mesh_ptr poly_mesh{ rcAllocPolyMesh() };

			if (poly_mesh == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate poly mesh");
				return nullptr;
			}

			if (!rcBuildPolyMesh(&ctx, contour_set, cfg.maxVertsPerPoly, *poly_mesh)) {
				kengine_assert_failed(r, "[Recast] Failed to build poly mesh");
				return nullptr;
			}

			return poly_mesh;
		}

		static poly_mesh_detail_ptr create_poly_mesh_detail(const entt::registry & r, rcContext & ctx, const rcConfig & cfg, const rcPolyMesh & poly_mesh, const rcCompactHeightfield & chf) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating poly mesh detail");

			poly_mesh_detail_ptr poly_mesh_detail{ rcAllocPolyMeshDetail() };
			if (poly_mesh_detail == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate poly mesh detail");
				return nullptr;
			}

			if (!rcBuildPolyMeshDetail(&ctx, poly_mesh, chf, cfg.detailSampleDist, cfg.detailSampleMaxError, *poly_mesh_detail)) {
				kengine_assert_failed(r, "[Recast] Failed to build poly mesh detail");
				return nullptr;
			}

			return poly_mesh_detail;
		}

		static data::nav_mesh_ptr create_nav_mesh(const entt::registry & r, const data::nav_mesh_data & data) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating nav mesh");

			data::nav_mesh_ptr nav_mesh{ dtAllocNavMesh() };
			if (nav_mesh == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate Detour navmesh");
				return nullptr;
			}

			const auto status = nav_mesh->init((unsigned char *)data.data.get(), data.size, 0);
			if (dtStatusFailed(status)) {
				kengine_assert_failed(r, "[Recast] Failed to init Detour navmesh");
				return nullptr;
			}

			return nav_mesh;
		}

		static data::nav_mesh_query_ptr create_nav_mesh_query(const entt::registry & r, const data::nav_mesh & params, const dtNavMesh & nav_mesh) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating nav mesh query");

			data::nav_mesh_query_ptr nav_mesh_query{ dtAllocNavMeshQuery() };

			if (nav_mesh_query == nullptr) {
				kengine_assert_failed(r, "[Recast] Failed to allocate Detour navmesh query");
				return nullptr;
			}

			const auto max_nodes = params.query_max_search_nodes;
			kengine_assert(r, 0 < max_nodes && max_nodes <= 65535);
			const auto status = nav_mesh_query->init(&nav_mesh, max_nodes);
			if (dtStatusFailed(status)) {
				kengine_assert_failed(r, "[Recast] Failed to init Detour navmesh query");
				return nullptr;
			}

			return nav_mesh_query;
		}

		static functions::get_path::callable get_path(const core::transform * model_transform, const data::nav_mesh & nav_mesh, const data::recast_nav_mesh & recast) noexcept {
			KENGINE_PROFILING_SCOPE;

			return [&, model_transform](entt::handle environment, const putils::point3f & start_world_space, const putils::point3f & end_world_space) {
				KENGINE_PROFILING_SCOPE;
				kengine_logf(
					*environment.registry(), verbose, "recast", "Getting path in [%u] from { %f, %f, %f } to { %f, %f, %f }",
					environment, start_world_space.x, start_world_space.y, start_world_space.z, end_world_space.x, end_world_space.y, end_world_space.z
				);
				static const dtQueryFilter filter;

				const auto model_to_world = glm::get_model_matrix(environment.get<core::transform>(), model_transform);
				const auto world_to_model = ::glm::inverse(model_to_world);

				const auto start = glm::convert_to_referencial(start_world_space, world_to_model);
				const auto end = glm::convert_to_referencial(end_world_space, world_to_model);

				functions::get_path_impl::path ret;

				const auto max_extent = std::max(nav_mesh.character_radius * 2.f, nav_mesh.character_height);
				const float extents[3] = { max_extent, max_extent, max_extent };

				dtPolyRef start_ref;
				float start_pt[3];
				auto status = recast.nav_mesh_query->findNearestPoly(start.raw, extents, &filter, &start_ref, start_pt);
				if (dtStatusFailed(status) || start_ref == 0) {
					kengine_log(*environment.registry(), verbose, "recast", "Failed to find nearest poly to start");
					return ret;
				}

				dtPolyRef end_ref;
				float end_pt[3];
				status = recast.nav_mesh_query->findNearestPoly(end.raw, extents, &filter, &end_ref, end_pt);
				if (dtStatusFailed(status) || end_ref == 0) {
					kengine_log(*environment.registry(), verbose, "recast", "Failed to find nearest poly to end");
					return ret;
				}

				dtPolyRef path[KENGINE_NAVMESH_MAX_PATH_LENGTH];
				int path_count = 0;
				status = recast.nav_mesh_query->findPath(start_ref, end_ref, start_pt, end_pt, &filter, path, &path_count, (int)putils::lengthof(path));
				if (dtStatusFailed(status)) {
					kengine_log(*environment.registry(), verbose, "recast", "Failed to find path");
					return ret;
				}

				ret.resize(ret.capacity());
				int straight_path_count = 0;

				static_assert(sizeof(putils::point3f) == sizeof(float[3]));
				status = recast.nav_mesh_query->findStraightPath(start_pt, end_pt, path, path_count, ret[0].raw, nullptr, nullptr, &straight_path_count, (int)ret.capacity());
				if (dtStatusFailed(status)) {
					kengine_log(*environment.registry(), verbose, "recast", "Failed to find straight path");
					return ret;
				}

				ret.resize(straight_path_count);
				for (auto & step : ret)
					step = glm::convert_to_referencial(step, model_to_world);

				return ret;
			};
		}

		enum flags {
			walk = 1,
		};
	};

	void build_recast_component(entt::registry & r, entt::entity e, const data::model_data & model_data, const data::nav_mesh & nav_mesh) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, verbose, "recast", "Building recast component for [%u]", e);

		kengine_assert(r, nav_mesh.verts_per_poly <= DT_VERTS_PER_POLYGON);

		async::task::string model_name;
		if (const auto asset = r.try_get<render::asset>(e))
			model_name = asset->file;
		else if (const auto name = r.try_get<core::name>(e))
			model_name = name->name;
		else
			model_name.set("%u", e);

		kengine::async::start_task(
			r, e,
			async::task::string("recast: load %s", model_name.c_str()),
			std::async(std::launch::async, [&r, e, &model_name, &model_data, &nav_mesh] {
				const putils::scoped_thread_name thread_name(putils::string<64>("Load navmesh for %s", model_name.c_str()));
				return build_recast_component::create_recast_mesh(model_name.c_str(), { r, e }, nav_mesh, model_data);
			})
		);
	}

	void process_built_recast_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "recast", "Processing built recast components");

		kengine::async::process_results<std::optional<data::recast_nav_mesh>>(r, [&](entt::entity e, std::optional<data::recast_nav_mesh> && opt) {
			if (!opt)
				return;

			const auto & recast = r.emplace<data::recast_nav_mesh>(e, std::move(*opt));
			const auto & nav_mesh = r.get<data::nav_mesh>(e);
			r.emplace<functions::get_path>(e, build_recast_component::get_path(r.try_get<core::transform>(e), nav_mesh, recast));
		});
	}
}
