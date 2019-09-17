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
    class PyComponent : public putils::Reflectible<PyComponent> {
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
        pmeta_get_class_name(PyComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute_private(&PyComponent::_scripts)
        );
        pmeta_get_methods(
                pmeta_reflectible_attribute(&PyComponent::attachScript),
                pmeta_reflectible_attribute(&PyComponent::removeScript)
        );
        pmeta_get_parents();
    };
}
