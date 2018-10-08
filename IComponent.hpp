#pragma once

#include <string>
#include "meta/type.hpp"

namespace kengine {
    class IComponent {
    public:
        virtual ~IComponent() = default;

        friend std::ostream & operator<<(std::ostream & s, const kengine::IComponent & obj) {
            s << obj.toString();
            return s;
        }

    private:
        virtual std::string toString() const = 0;

    public:
        virtual pmeta::type_index getType() const noexcept = 0;
    };

    template<typename T>
    using is_component = std::is_base_of<IComponent, T>;
}
