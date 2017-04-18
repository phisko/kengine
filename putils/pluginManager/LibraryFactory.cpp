#include "LibraryFactory.hpp"

namespace putils
{
    static std::string addLibToPath(const std::string &name, const std::string &extension) noexcept
    {
        static std::regex end("^.*\\" + extension + "$");

        if (!std::regex_match(name, end))
        {
            std::string path = "";
            std::string lib = name;

            auto it = name.find_last_of("/");
            if (it != std::string::npos)
            {
                path = name.substr(0, it + 1);
                lib = name.substr(it + 1);
            }
            return path + "lib" + lib + extension;
        }
        else
            return name;
    }

    putils::Library *LibraryFactory::make(const std::string &name)
    {
        static std::unordered_map<std::string, std::unique_ptr<Library>> _register;

        if (_register.find(name) != _register.end())
            return _register.at(name).get();

#ifdef MINGW
        auto lib = std::make_unique<WindowsLibrary>(addLibToPath(name, ".dll"));
        _register.emplace(name, std::move(lib));
#elif defined(_WIN32)
        static std::regex		end(R"(^.*\.dll$)");

        auto lib = std::make_unique<WindowsLibrary>(std::regex_match(name, end) ? name : name + ".dll");
        _register.emplace(name, std::move(lib));
#elif defined(__unix__)
        auto lib = std::make_unique<UnixLibrary>(addLibToPath(name, ".so"));
        _register.emplace(name, std::move(lib));
#endif
        return _register.at(name).get();
    }
}
