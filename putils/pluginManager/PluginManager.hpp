#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <fstream>
#include <regex>
#include <iostream>
#include "Library.hpp"

namespace putils
{
//
// PluginManager which either loads all libraries in a folder
// or loads libraries contained in a CSV config file
// Offers functions to execute a given function in all libraries,
// in all libraries of certain categories, with or without
// return values
//
    class PluginManager
    {
        // Constructor
    public:
        // Loads a CSV config file formatted "category,[category...];path_to_library"
        // Alternatively, if dir is true, loads any .dll or .so file in the directory specified by path
        // Unix libraries should be .so files and Windows libraries should be .dll files
        PluginManager(const std::string &path, bool dir = true) noexcept;

    public:
        // In each plugin, execute the [name] function, taking P as parameter
        template<typename ...P>
        void execute(const std::string &name, P &&...params) noexcept
        {
            for (auto &pair : _libraries)
                for (auto &plugin : pair.second)
                    plugin->execute<void>(name, std::forward<P>(params)...);
        }

        // In plugins of the specified [category], execute the [name] function, taking P as parameter
        template<typename ...P>
        void executeInCategory(const std::string &name, const std::string &category, P &&...params) noexcept
        {
            for (auto &plugin : _libraries[category])
                plugin->execute<void>(name, std::forward<P>(params)...);
        }

        // In each plugin, execute the [name] function, returning a T and taking a P as parameter
        // Returns a vector of all values returned
        template<typename T, typename ...P>
        std::vector<T> executeWithReturn(const std::string &name, P &&...params) noexcept
        {
            std::vector<T> ret;

            for (auto &pair : _libraries)
                for (auto &plugin : pair.second)                            // pair.first is category
                {
                    auto func = plugin->loadMethod<T, P...>(name);
                    if (func != nullptr)
                        ret.push_back(func(std::forward<P>(params)...));
                }

            return ret;
        }

        // In plugins of the specified [category], execute the [name] function, returning a T and taking a P as parameter
        // Returns a vector of all values returned
        template<typename T, typename ...P>
        std::vector<T>
        executeInCategoryWithReturn(const std::string &name, const std::string &category, P &&...params) noexcept
        {
            std::vector<T> ret;

            for (auto &plugin : _libraries[category])
            {
                auto func = plugin->loadMethod<T, P...>(name);
                if (func != nullptr)
                    ret.push_back(func(std::forward<P>(params)...));
            }

            return ret;
        }

        // Constructor utilities, load from CSV or directory
    private:
        void loadCSV(const std::string &path) noexcept;

        void loadDir(const std::string &path) noexcept;

        // Add library to corresponding categories
    private:
        void fillCategories(putils::Library *plugin, std::string &categories) noexcept;

        // A category is a vector of libraries
    private:
        using Category = std::vector<putils::Library *>;

        // Attributes
    private:
        std::unordered_map<std::string, Category> _libraries;

        // Coplien
    public:
        PluginManager(const PluginManager &) = delete;

        PluginManager &operator=(const PluginManager &) = delete;
    };
}
