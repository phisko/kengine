#pragma once

// meta
#include "meta/type.hpp"

// kengine impl
#include "ID.hpp"
#include "Mutex.hpp"
#include "ComponentSettings.hpp"

namespace kengine::impl {
	struct KENGINE_CORE_EXPORT ComponentMetadata {
		ID id = INVALID_ID;
		putils::meta::type_index type;
		virtual void reset(ID id) noexcept = 0;
		virtual ~ComponentMetadata() noexcept = default;
	};

	template<typename Comp>
	struct Metadata : ComponentMetadata {
        Metadata(Metadata * & singleton) noexcept : singleton(singleton) {}
        ~Metadata() noexcept;

        Metadata * & singleton;
		componentSettings::map<Comp> _map;
		Mutex _mutex;
		void reset(ID id) noexcept final;
	};
}