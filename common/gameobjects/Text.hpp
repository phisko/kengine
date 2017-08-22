#pragma once

#include "GameObject.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/components/GUIComponent.hpp"

namespace kengine
{
    class Text : public kengine::GameObject
    {
    public:
        Text(std::string_view name, std::string_view text = "", std::size_t fontSize = 18, std::string_view font = "")
                : kengine::GameObject(name)
        {
            attachComponent<kengine::GUIComponent>(text, fontSize, font);
            attachComponent<kengine::TransformComponent3d>();
        }

    public:
        static const auto get_class_name() { return pmeta_nameof(Text); }
    };
}