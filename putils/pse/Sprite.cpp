#include <iostream>
#include "Sprite.hpp"

namespace pse
{
    std::unordered_map<std::string, std::unique_ptr<sf::Texture> >    Sprite::textures;

    Sprite::Sprite(const std::string &texture, const sf::Vector2f &pos, const sf::Vector2f &size) noexcept
            :
            _textureFile(texture),
            _size(size)
    {
        Sprite::setTexture(texture);
        setPosition(pos);
        setSize(_size);
    }

    std::unique_ptr<ViewItem> Sprite::copy() const noexcept
    {
        return std::make_unique<Sprite>(_textureFile, _size, sf::Vector2f(0, 0));
    }

    void Sprite::setTexture(const std::string &texture) noexcept
    {
        if (textures.find(texture) == textures.end())
        {
            auto t = std::make_unique<sf::Texture>();

            if (!t->loadFromFile(texture))
            {
                std::cerr << "Error loading texture '" << texture << "'" << std::endl;
                return;
            }
            t->setSmooth(true);

            textures[texture] = std::move(t);
        }

        _texture = textures[texture].get();
        _sprite.setTexture(*_texture);
    }
}