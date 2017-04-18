#pragma once

#include <unordered_map>
#include "SFML/Graphics.hpp"
#include "ViewItem.hpp"

namespace pse
{
    //
    // SFML Text class
    //
    class Text : public ViewItem
    {
        // Constructor
    public:
        Text(const sf::String &str, const sf::Vector2f &pos = {0, 0}, const sf::Color &color = sf::Color::White,
             unsigned int textSize = 18, const std::string &font = "resources/fonts/arial.ttf",
             const sf::Text::Style &style = sf::Text::Regular) noexcept;

        // ViewItem functions
    public:
        std::unique_ptr<ViewItem> copy() const noexcept override;

        void draw(sf::RenderWindow &window) noexcept override { window.draw(getDrawable()); }

        const sf::Drawable &getDrawable() noexcept override { return _text; }

        sf::Vector2f getSize() const noexcept override;

        sf::Transformable &getTransformable() noexcept override { return _text; }

        // Setters
    public:
        void setString(const sf::String &str) noexcept;

        void setColor(const sf::Color &color) noexcept;

        void setTextSize(unsigned int textSize) noexcept;

        void setFont(const std::string &font) noexcept;

        void setStyle(const sf::Text::Style &style) noexcept;

        // Attributes
    private:
        sf::Text _text;
        sf::String _str;
        sf::Color _color;
        unsigned int _textSize;
        sf::Font _font;
        std::string _fontFile;
        sf::Text::Style _style;

    private:
        static std::unordered_map<std::string, sf::Font *> fonts;

        // Coplien
    public:
        Text(const Text &) = delete;

        Text &operator=(const Text &) = delete;
    };
}
