#pragma once

#include <functional>
#include <type_traits>

namespace putils
{
    template<typename T, bool = std::is_scalar<T>::value>
    class RAII;

//
// RAII template
// Create one by giving it a resource and a function to release that resource
// Can be used just like the resource by using the implicit conversion operators
//
    template<typename T>
    class RAII<T, false>
    {
    public:
        RAII(T &&res, std::function<void(T &)> &&dtor = [](T &) {})
                : _res(std::forward<T>(res)), _dtor(std::move(dtor)) {}

    public:
        RAII(RAII<T> &&other)
                : _res(std::move(other._res)), _dtor(std::move(other._dtor))
        {
            other._release = false;
        }

        RAII &operator=(RAII<T> &&other)
        {
            _res = std::move(other._res);
            _dtor = std::move(other._dtor);
            other._release = false;
            return *this;
        }

    public:
        RAII(const RAII<T> &) = delete;

        RAII<T> &operator=(const RAII<T> &) = delete;

    public:
        ~RAII()
        {
            if (_release)
                _dtor(_res);
        }

    public:
        T &get() noexcept { return _res; }

        const T &get() const noexcept { return _res; }

        operator T &() { return _res; }

        operator const T &() const { return _res; }

    public:
        template<typename Arg>
        void operator=(Arg &&val) { _res = std::forward<Arg>(val); }

    private:
        T _res;
        bool _release{true};
        std::function<void(T &)> _dtor;
    };

    template<typename T>
    class RAII<T, true>
    {
    public:
        RAII(T res, std::function<void(T)> &&dtor = [](T) {})
                : _res(res), _dtor(std::move(dtor)) {}

    public:
        RAII(RAII<T> &&other)
                : _res(other._res), _dtor(std::move(other._dtor))
        {
            other._release = false;
        }

        RAII &operator=(RAII<T> &&other)
        {
            _res = other._res;
            _dtor = std::move(other._dtor);
            other._release = false;
            return *this;
        }

    public:
        ~RAII()
        {
            if (_release)
                _dtor(_res);
        }

    public:
        RAII(const RAII<T> &) = delete;

        RAII<T> &operator=(const RAII<T> &) = delete;

    public:
        T &get() noexcept { return _res; }

        const T &get() const noexcept { return _res; }

        operator T() { return _res; }

        operator const T() const { return _res; }

    public:
        void operator=(T val) { _res = val; }

    private:
        T _res;
        bool _release{true};
        std::function<void(T)> _dtor;
    };
}
