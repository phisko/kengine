#pragma once

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/to_string.hpp"
#include "putils/reflection_helpers/json_helper.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine instance
#include "kengine/instance/data/instance.hpp"

// kengine instance/find_model
#include "kengine/instance/find_model/data/instance_of.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::instance::find_model {
	template<typename InstanceOf>
	struct system {
		using model_component = typename InstanceOf::model_component;
		static inline const auto log_category = std::string("instance_find_model_by_") + putils::reflection::get_class_name<model_component>();

		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, InstanceOf> processor{ r, putils_forward_to_this(find_or_create_model) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category.c_str(), "Initializing");
			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category.c_str(), "Executing");
			processor.process();
		}

		void find_or_create_model(entt::entity e, const InstanceOf & model_reference) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.get_or_emplace<instance>(e);
			if (comp.model != entt::null) {
				kengine_logf(r, verbose, log_category.c_str(), "[%u] already has a model ([%u])", comp.model);
				return;
			}

			kengine_logf(r, verbose, log_category.c_str(), "Looking for model for [%u] (model %s: %s)", e, putils::reflection::get_class_name<model_component>(), to_string(model_reference.model).c_str());

			for (const auto & [model, model_comp] : r.view<model_component>().each())
				if (is_equal(model_comp, model_reference.model)) {
					comp.model = model;
					kengine_logf(r, verbose, log_category.c_str(), "Found existing model ([%u])", model);
					return;
				}

			const auto model = r.create();
			kengine_logf(r, log, log_category.c_str(), "Created new model [%u] for %s %s", model, putils::reflection::get_class_name<model_component>(), to_string(model_reference.model).c_str());
			r.emplace<model_component>(model, model_reference.model);
			comp.model = model;
		}

		static std::string to_string(const model_component & comp) noexcept {
			if constexpr (putils::streamable<model_component, std::stringstream>)
				return putils::to_string(comp);
			else
				return putils::reflection::to_json(comp).dump();
		}

		template<typename T>
		static bool is_equal(const T & lhs, const T & rhs) noexcept {
			if constexpr (std::equality_comparable<T>)
				return lhs == rhs;
			else {
				static_assert(putils::reflection::has_attributes<T>());
				return !putils::reflection::for_each_attribute<T>([&](const auto & attr) noexcept {
					return !is_equal(lhs.*(attr.ptr), rhs.*(attr.ptr));
				});
			}
		}
	};
}