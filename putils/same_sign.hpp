#pragma once

namespace putils
{
    // Check if two values have the same sign
    template<typename ValueType>
    bool sameSign(ValueType a, ValueType b) { return a == 0 || b == 0 || ((a < 0) == (b < 0)); }
}