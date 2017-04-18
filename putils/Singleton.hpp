#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <type_traits>

namespace putils
{
    //
    // Singleton class template: provides a global point of access to an object
    // That object can be specified with "set", or it will be default constructed when
    // "get" is called
    // Template is specialized for abstract classes, in which case "get" will return NULL
    // if "set" wasnt' called, as abstract classes can't be instantiated
    //
    template<typename T, bool = std::is_abstract<T>::value>
    class Singleton;

    template<typename T>
    class Singleton<T, false>
    {
        // Set the global object
    public:
        static void set(T *instance) noexcept { _instance = std::shared_ptr<T>(instance); }

        static void set(const std::string &name, T *instance) noexcept
        {
            _registry.emplace(name, std::shared_ptr<T>(instance));
        }

        // Get the global object
    public:
        static T *get() noexcept
        {
            if (!_instance.get())
                _instance = std::make_shared<T>();
            return _instance.get();
        }

        static T *get(const std::string &name) noexcept
        {
            try
            {
                return _registry.at(name).get();
            }
            catch (...)
            {
                _registry.emplace(name, std::make_shared<T>());
                return _registry.at(name).get();
            }
        }

        // Attributes
    private:
        static std::shared_ptr<T> _instance;
        static std::unordered_map<std::string, std::shared_ptr<T> > _registry;

        // Constructor and Coplien
    public:
        Singleton() = delete;

        Singleton(const Singleton &) = delete;

        Singleton operator=(const Singleton &) = delete;
    };

    template<typename T>
    class Singleton<T, true>
    {
    public:
        static void set(T *instance) noexcept { _instance = std::shared_ptr<T>(instance); }

        static void set(const std::string &name, T *instance) noexcept
        {
            _registry.emplace(name, std::shared_ptr<T>(instance));
        }

    public:
        static T *get() noexcept { return _instance.get(); }

    public:
        static T *get(const std::string &name) noexcept
        {
            try
            {
                return _registry.at(name).get();
            }
            catch (...)
            {
                return NULL;
            }
        }

    private:
        static std::shared_ptr<T> _instance;
        static std::unordered_map<std::string, std::shared_ptr<T> > _registry;

    public:
        Singleton() = delete;

        Singleton(const Singleton &) = delete;

        Singleton operator=(const Singleton &) = delete;
    };

    template<typename T>
    std::shared_ptr<T>    Singleton<T, true>::_instance = nullptr;

    template<typename T>
    std::shared_ptr<T>    Singleton<T, false>::_instance = nullptr;
}