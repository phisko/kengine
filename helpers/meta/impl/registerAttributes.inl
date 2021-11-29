#pragma once

#include "meta/Attributes.hpp"

namespace kengine {
    namespace impl {
		template<typename T>
		static void fillAttributes(meta::Attributes::AttributeMap & attributes) {
			putils::reflection::for_each_attribute<T>([&](std::string_view name, const auto member) {
				using MemberType = putils::MemberType<decltype(member)>;

				auto & attr = attributes[std::string(name)];
				attr.size = sizeof(MemberType);
				attr.offset = putils::member_offset(member);
				fillAttributes<MemberType>(attr.attributes);
			});
		}
	}

    template<typename ... Comps>
    void registerAttributes() noexcept {
        putils::for_each_type<Comps...>([](const auto & t) {
            using Type = putils_wrapped_type(t);
			if constexpr (putils::reflection::has_class_name<Type>())
				kengine_logf(Log, "Init/registerMetaComponents", "Registering Attributes for %s", putils::reflection::get_class_name<Type>());

        	auto type = typeHelper::getTypeEntity<Type>();
        	meta::Attributes attributes;
        	impl::fillAttributes<Type>(attributes.attributes);
        	type += std::move(attributes);
        });
    }
}
