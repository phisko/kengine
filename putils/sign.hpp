#pragma once

namespace putils
{
    inline int sign(int a) { return a < 0 ? -1 : a > 0 ? 1 : 0; }
}