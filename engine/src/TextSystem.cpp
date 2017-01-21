//
// Created by antry on 1/21/17.
//

#include "TextSystem.hpp"

TextSystem::TextSystem(TextComponent &comp)
{
}

void TextSystem::execute()
{
    for(auto const& item : textcomponents)
        std::cout << item.toString() << std::endl;
}

void TextSystem::registerComponent(TextComponent& comp)
{
    textcomponents.push_back(comp);
}
