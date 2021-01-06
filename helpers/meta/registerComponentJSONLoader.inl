#include "registerComponentJSONLoader.hpp"

#include "kengine.hpp"
#include "reflection/json_helper.hpp"
#include "meta/LoadFromJSON.hpp"
#include "helpers/typeHelper.hpp"

namespace kengine {
	namespace impl {
		template<typename Component>
		static void loadJSONComponent(const putils::json & jsonEntity, Entity & e) noexcept {
			const auto it = jsonEntity.find(putils::reflection::get_class_name<Component>());
			if (it == jsonEntity.end())
				return;
			auto & comp = e.attach<Component>();
			putils::reflection::fromJSON(*it, comp);
		}
	}

	template<typename ... Comps>
	void registerComponentJSONLoader() noexcept {
		putils::for_each_type<Comps...>([](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			auto type = typeHelper::getTypeEntity<Type>();
			type += meta::LoadFromJSON{ impl::loadJSONComponent<Type> };
		});
	}
}
