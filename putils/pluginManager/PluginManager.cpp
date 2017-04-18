#include "PluginManager.hpp"
#include "Directory.hpp"
#include "LibraryFactory.hpp"

namespace putils
{
    PluginManager::PluginManager(const std::string &path, bool dir) noexcept
    {
        if (dir == false)
            loadCSV(path);
        else
            loadDir(path);
    }

    void PluginManager::loadDir(const std::string &path) noexcept
    {
#ifdef _WIN32
        static std::regex			freg("^.*\\.dll$");
#else
        static std::regex freg("^.*\\.so");
#endif

        putils::Directory dir(path);

        dir.for_each([this](const putils::ADirectory::File &f)
        {
            if (std::regex_match(f.fullPath, freg))
            {
                try
                {
                    auto plugin = putils::LibraryFactory::make(f.fullPath);
                    _libraries["all"].push_back(plugin);
                }
                catch (std::runtime_error &e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
        });
    }

    void PluginManager::loadCSV(const std::string &path) noexcept
    {
        std::ifstream file(path);
        std::string buff;

        std::regex reg("^([^;]*);?(.*)$", std::regex::extended);

        std::regex comment("^;;.*$", std::regex::extended);

        while (std::getline(file, buff))
        {
            try
            {
                if (std::regex_match(buff, comment))
                    continue;

                std::smatch result;
                std::regex_match(buff, result, reg);

                std::string name(result[1]);
                std::string categories(result[2]);

                if (name.length())
                    name = "./" + name;

                try
                {
                    auto plugin = putils::LibraryFactory::make(name);
                    fillCategories(plugin, categories);
                }
                catch (std::runtime_error &e)
                {
                    std::cerr << e.what() << std::endl;
                }
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    void PluginManager::fillCategories(putils::Library *plugin, std::string &categories) noexcept
    {
        std::regex reg("^([^,]*),?(.*)$", std::regex::extended);

        while (categories.length() > 0)
        {
            try
            {
                std::smatch result;

                std::regex_match(categories, result, reg);

                std::string category(result[1]);
                categories = result[2];

                _libraries[category].push_back(plugin);
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
    }
}
