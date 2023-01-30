#include "register_everything.hpp"

// reflection
#include "putils/reflection.hpp"

#include "register_metadata.hpp"
#include "register_meta_components.hpp"
#include "register_with_script_languages.hpp"

namespace kengine {
	template<typename... Comps>
	void register_everything(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		register_metadata<Comps...>(r);
		register_with_script_languages<true, Comps...>(r);
		register_meta_components<Comps...>(r);
	}
}