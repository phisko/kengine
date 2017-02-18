//
// Created by antry on 1/21/17.
//

#include "TextSystem.hpp"

const ComponentMask TextSystem::Mask = ComponentMask::Ui;

TextSystem::TextSystem()
{
}

void TextSystem::execute()
{
    for(auto const& item : _textComponents)
        std::cout << item.toString() << std::endl;
}

void TextSystem::registerComponent(Component& comp)
{
    _textComponents.push_back(comp);
}
