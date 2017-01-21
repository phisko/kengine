//
// Created by antry on 1/21/17.
//

#ifndef KENGINE_TEXTSYSTEM_HPP
# define KENGINE_TEXTSYSTEM_HPP

#include <vector>
# include "TextComponent.hpp"
# include "ISystem.hpp"

class TextSystem : ISystem
{
public:
    TextSystem(TextComponent& comp);
    ~TextSystem(){};

public:
    void execute();
    void registerComponent(TextComponent& comp);

private:
    std::vector<TextComponent> textcomponents;
};

#endif //KENGINE_TEXTSYSTEM_HPP
