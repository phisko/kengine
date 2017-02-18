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
    for (auto const& item : _gameObjects)
        std::cout << item->toString() << std::endl;
}

void TextSystem::registerGameObject(GameObject &gameObject)
{
    _gameObjects.push_back(&gameObject);
}
