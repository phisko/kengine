#pragma once

#include "ComponentMetadata.hpp"

#ifndef KENGINE_COMPONENT_CHUNK_SIZE
# define KENGINE_COMPONENT_CHUNK_SIZE 64
#endif

namespace kengine::impl {
	template<typename Comp>
	class Component {
	public:
        static Comp & get(ID entity) noexcept;

        template<typename InitialValue = std::nullptr_t>
		static Comp & set(ID entity, InitialValue && initialValue = nullptr) noexcept;

		static ID id() noexcept;

		static Metadata<Comp> & metadata() noexcept;
	};

	void addComponent(EntityID entity, ComponentID component) noexcept;
	void removeComponent(EntityID entity, ComponentID component) noexcept;
}

#include "Component.inl"