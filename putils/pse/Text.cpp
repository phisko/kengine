#include <iostream>
#include "Text.hpp"

namespace pse
{
    std::unordered_map<std::string, sf::Font *>    Text::fonts;

    Text::Text(const sf::String &str, const sf::Vector2f &pos, const sf::Color &color, unsigned int textSize,
               const std::string &font, const sf::Text::Style &style) noexcept
            :
            _fontFile(font)
    {
        setFont(font);
        setString(str);
        setColor(color);
        setTextSize(textSize);
        setPosition(pos);
        setStyle(style);
    }

    std::unique_ptr<ViewItem> Text::copy() const noexcept
    {
        return std::make_unique<Text>(_str, sf::Vector2f(0, 0), _color, _textSize, _fontFile, _style);
    }

    sf::Vector2f Text::getSize() const noexcept
    {
        sf::FloatRect rect = _text.getLocalBounds();
        return { rect.width, rect.height };
    }

    void Text::setString(const sf::String &str) noexcept
    {
        _str = str;
        _text.setString(_str);
    }

    void Text::setColor(const sf::Color &color) noexcept
    {
        _color = color;
        _text.setColor(color);
    }

    void Text::setTextSize(unsigned int textSize) noexcept
    {
        _textSize = textSize;
        _text.setCharacterSize(_textSize);
    }

    void Text::setFont(const std::string &font) noexcept
    {
        if (fonts.find(font) == fonts.end())
        {
            sf::Font *f = new sf::Font();
            if (!(f->loadFromFile(font)))
            {
                std::cerr << "Error loading font '" + font + "'" << std::endl;
                return;
            }
            fonts[font] = f;
        }

        _font = *fonts[font];
        _text.setFont(_font);
    }

    void Text::setStyle(const sf::Text::Style &style) noexcept
    {
        _style = style;
        _text.setStyle(_style);
    }
}
