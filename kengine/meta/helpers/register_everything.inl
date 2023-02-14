#include "register_everything.hpp"

// reflection
#include "putils/reflection.hpp"

#include "register_metadata.hpp"
#include "register_meta_components.hpp"
#include "register_storage.hpp"
#include "register_with_script_languages.hpp"

namespace kengine {
	template<typename... Comps>
	void register_everything(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "register_everything", "Registering types");

		register_storage<Comps...>(r);
		register_metadata<Comps...>(r);
		register_with_script_languages<true, Comps...>(r);
		register_meta_components<Comps...>(r);
	}
}