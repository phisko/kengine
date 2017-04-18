#pragma once

namespace putils
{
    template<typename Attr>
    class OperatorForwarding
    {
        public:
            OperatorForwarding(Attr &attr) : _attr(attr) {}

        public:
            template<typename T>
            decltype(auto) operator+=(const T &rhs) { return _attr += rhs; }

            template<typename T>
            decltype(auto) operator-=(const T &rhs) { return _attr -= rhs; }

            template<typename T>
            decltype(auto) operator*=(const T &rhs) { return _attr *= rhs; }

            template<typename T>
            decltype(auto) operator/=(const T &rhs) { return _attr /= rhs; }

            template<typename T>
            decltype(auto) operator%=(const T &rhs) { return _attr %= rhs; }

            template<typename T>
            decltype(auto) operator&=(const T &rhs) { return _attr &= rhs; }

            template<typename T>
            decltype(auto) operator|=(const T &rhs) { return _attr |= rhs; }

            template<typename T>
            decltype(auto) operator^=(const T &rhs) { return _attr ^= rhs; }

            template<typename T>
            decltype(auto) operator<<=(const T &rhs) { return _attr <<= rhs; }

            template<typename T>
            decltype(auto) operator>>=(const T &rhs) { return _attr >>= rhs; }

            decltype(auto) operator++() { return ++_attr; }

            decltype(auto) operator++(int) { return _attr++; }

            decltype(auto) operator--() { return --_attr; }

            decltype(auto) operator--(int) { return _attr--; }

            decltype(auto) operator+() { return +_attr; }

            template<typename T>
            decltype(auto) operator+(const T &rhs) { return _attr + rhs; }

            template<typename T>
            decltype(auto) operator-(const T &rhs) { return _attr - rhs; }

            template<typename T>
            decltype(auto) operator*(const T &rhs) { return _attr * rhs; }

            template<typename T>
            decltype(auto) operator/(const T &rhs) { return _attr / rhs; }

            template<typename T>
            decltype(auto) operator%(const T &rhs) { return _attr % rhs; }

            template<typename T>
            decltype(auto) operator~() { return ~_attr; }

            template<typename T>
            decltype(auto) operator&(const T &rhs) { return _attr & rhs; }

            template<typename T>
            decltype(auto) operator|(const T &rhs) { return _attr | rhs; }

            template<typename T>
            decltype(auto) operator^(const T &rhs) { return _attr ^ rhs; }

            template<typename T>
            decltype(auto) operator<<(const T &rhs) { return _attr << rhs; }

            template<typename T>
            decltype(auto) operator>>(const T &rhs) { return _attr >> rhs; }

            decltype(auto) operator!() { return !_attr; }

            template<typename T>
            decltype(auto) operator&&(const T &rhs) { return _attr && rhs; }

            template<typename T>
            decltype(auto) operator||(const T &rhs) { return _attr || rhs; }

            template<typename T>
            decltype(auto) operator==(const T &rhs) { return _attr == rhs; }

            template<typename T>
            decltype(auto) operator!=(const T &rhs) { return _attr != rhs; }

            template<typename T>
            decltype(auto) operator<(const T &rhs) { return _attr < rhs; }

            template<typename T>
            decltype(auto) operator>(const T &rhs) { return _attr > rhs; }

            template<typename T>
            decltype(auto) operator<=(const T &rhs) { return _attr <= rhs; }

            template<typename T>
            decltype(auto) operator>=(const T &rhs) { return _attr >= rhs; }

            template<typename T>
            decltype(auto) operator[](const T &rhs) { return _attr[rhs]; }

            decltype(auto) operator-() { return -_attr; }

            decltype(auto) operator*() { return *_attr; }

            decltype(auto) operator->() { return _attr; }

            template<typename T>
            decltype(auto) operator->*(const T &rhs) { return _attr->*rhs; }

            template<typename T>
            decltype(auto) operator,(const T &rhs) { return _attr, rhs; }

            template<typename ...T>
            decltype(auto) operator()(T &&...args) { return _attr(std::forward<T>(args)...); }

        private:
            Attr    &_attr;
    };
}
