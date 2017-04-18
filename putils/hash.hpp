#pragma once

namespace putils
{
    // Template to give as a 3rd parameter to unordered_map when using an enum as key
    struct EnumHash
    {
        template<typename T>
        std::size_t operator()(T t) const noexcept
        {
            return static_cast<std::size_t>(t);
        }
    };

    // Template to combine two hashes (to use pairs as map keys, for instance)
    template<class T>
    inline void hash_combine(std::size_t &seed, const T &v) noexcept
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    // Template to give as a 3rd parameter to unordered_map when using a pair as key
    struct PairHash
    {
        template<typename T, typename U>
        auto operator()(const std::pair<T, U> &t) const noexcept
        {
            std::hash<T> hasher;
            auto ret(hasher(t.first));
            hash_combine(ret, t.second);
            return ret;
        }
    };
}
