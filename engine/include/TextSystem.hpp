//
// Created by antry on 1/21/17.
//

#ifndef KENGINE_TEXTSYSTEM_HPP
# define KENGINE_TEXTSYSTEM_HPP

# include <vector>
# include "TextComponent.hpp"
# include "ISystem.hpp"

class TextSystem : public ISystem
{
public:
    static const ComponentMask Mask;

public:
    TextSystem();

    ~TextSystem()
    {};

public:
    void execute();
    void registerGameObject(GameObject& gameObject);

private:
    std::vector<GameObject*> _gameObjects;
};

#endif //KENGINE_TEXTSYSTEM_HPP
