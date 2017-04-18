#pragma once

#include "meta/type.hpp"

namespace putils
{
    struct ADataPacket
    {
        virtual ~ADataPacket() {}

        ADataPacket(pmeta::type_index type) : type(type) {}

        pmeta::type_index type;
    };

    template<typename T>
    struct DataPacket : ADataPacket
    {
        DataPacket(const T &data)
                : ADataPacket(pmeta::type<T>::index), data(data)
        {}

        const T &data;
    };
}
