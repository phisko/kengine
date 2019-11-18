#pragma once

#ifndef KENGINE_MAX_PYTHON_SCRIPT_PATH
# define KENGINE_MAX_PYTHON_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_PYTHON_SCRIPTS
# define KENGINE_MAX_PYTHON_SCRIPTS 8
#endif

#include "string.hpp"
#include "vector.hpp"

namespace kengine {
    class PyComponent {
    public:
		static constexpr char stringName[] = "PyComponentString";
		using script = putils::string<KENGINE_MAX_PYTHON_SCRIPT_PATH, stringName>;
		static constexpr char vectorName[] = "PyComponentVector";
		using script_vector = putils::vector<script, KENGINE_MAX_PYTHON_SCRIPTS, vectorName>;

    public:
        PyComponent(const script_vector & scripts = {}) : _scripts(scripts) {}

		PyComponent(const PyComponent &) = default;
		PyComponent & operator=(const PyComponent &) = default;
		PyComponent(PyComponent &&) = default;
		PyComponent & operator=(PyComponent &&) = default;

    public:
        void attachScript(const char * file) noexcept { _scripts.push_back(file); }

        void removeScript(const char * file) noexcept {
            _scripts.erase(std::find(_scripts.begin(), _scripts.end(), file));
        }

    public:
        const script_vector & getScripts() const noexcept { return _scripts; }

    private:
        script_vector _scripts;

        /*
         * Reflectible
         */
    public:
        putils_reflection_class_name(PyComponent);
        putils_reflection_attributes(
                putils_reflection_attribute_private(&PyComponent::_scripts)
        );
        putils_reflection_methods(
                putils_reflection_attribute(&PyComponent::attachScript),
                putils_reflection_attribute(&PyComponent::removeScript)
        );
    };
}
