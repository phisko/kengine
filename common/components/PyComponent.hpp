#pragma once

#include <string>
#include "SerializableComponent.hpp"

namespace kengine {
    class PyComponent : public putils::Reflectible<PyComponent>,
                         public SerializableComponent<PyComponent> {
    public:
        PyComponent(const std::vector<std::string> & scripts = {})
                : _scripts(scripts) {
        }

    public:
        void attachScript(const std::string & file) noexcept { _scripts.push_back(file); }

        void removeScript(const std::string & file) noexcept {
            _scripts.erase(std::find(_scripts.begin(), _scripts.end(), file));
        }

    public:
        const std::vector<std::string> & getScripts() const noexcept { return _scripts; }

    private:
        const std::string type = pmeta_nameof(PyComponent);
        std::vector<std::string> _scripts;

        /*
         * Reflectible
         */
    public:
        pmeta_get_class_name(PyComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&PyComponent::type),
                pmeta_reflectible_attribute_private(&PyComponent::_scripts)
        );
        pmeta_get_methods(
                pmeta_reflectible_attribute(&PyComponent::attachScript),
                pmeta_reflectible_attribute(&PyComponent::removeScript)
        );
        pmeta_get_parents();
    };
}
