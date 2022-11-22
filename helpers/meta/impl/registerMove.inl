#include "registerMove.hpp"

// kengine meta
#include "meta/Move.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerMove() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementationWithPredicate<meta::Move, std::is_copy_constructible, Comps...>(
			[](const auto t, entt::handle src, entt::handle dst) noexcept {
				using T = putils_wrapped_type(t);
				if constexpr (!std::is_empty<T>()) {
					auto & srcComponent = src.get<T>();
					dst.emplace_or_replace<T>(std::move(srcComponent));
					src.erase<T>();
				}
				else
					dst.emplace<T>();
			}
		);
	}
}