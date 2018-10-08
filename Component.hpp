#pragma once

#include "IComponent.hpp"

namespace kengine {
    template<typename CRTP>
    class Component : public IComponent {
    public:
        pmeta::type_index getType() const noexcept final { return pmeta::type<CRTP>::index; }
    };
}
