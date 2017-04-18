#pragma once

#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#ifdef PSE_TGUI
#include <TGUI/Gui.hpp>
#include <TGUI/Widget.hpp>
#endif

#include "ViewItem.hpp"

//
// Phiste SFML Engine
//
namespace pse
{
    class Engine
    {
        // Constructor
    public:
        Engine(size_t screenWidth, size_t screenHeight,
               const std::string &screenName,
               double refreshRate = 120) noexcept;

        // Destructor
    public:
        ~Engine() {}

        // Items getters and setters
    public:
        void addItem(ViewItem *item, size_t height = 0) { _items.push_back(std::make_pair(item, height)); }

        void removeItem(ViewItem *item)
        {
            _items.erase(std::find_if(_items.begin(), _items.end(),
                    [item](auto &p) { return p.first == item; })
            );
        }

        // Update the engine
        // clear: whether or not the screen should be splashed black
    public:
        void update(bool clear = true) noexcept;

        // Poll event
    public:
        bool pollEvent(sf::Event &e) noexcept
        {
            bool ret = _window.pollEvent(e);

#ifdef PSE_TGUI
            if (ret)
                _tgui.handleEvent(e);
#endif

            return ret;
        }

        // Splash a color on screen
    public:
        void displayColor(const sf::Color &color) noexcept { _window.clear(color); }

    private:
        void refreshMovingItems() noexcept;

        void drawItems() noexcept;

        // Approximation of whether two positions are the same
    public:
        static bool isSamePosition(const sf::Vector2f &p1, const sf::Vector2f &p2) noexcept
        {
            return ((-5 < (p1.x - p2.x) && (p1.x - p2.x) < 5) &&
                    (-5 < (p1.y - p2.y) && (p1.y - p2.y) < 5));
        }

        // Access sf::RenderWindow
    public:
        sf::RenderWindow &getRenderWindow() noexcept { return _window; }

        // Attributes
    private:
        sf::RenderWindow _window;
        std::vector<std::pair<ViewItem *, size_t>> _items;
        putils::Timer _refreshTimer;

#ifdef PSE_TGUI
    public:
        template<typename T, typename = std::enable_if_t<std::is_base_of<tgui::Widget, T>::value>>
        void addItem(const std::shared_ptr<T> &widget, const std::string &name = "")
        { _tgui.add(widget, name); }

        template<typename T, typename = std::enable_if_t<std::is_base_of<tgui::Widget, T>::value>>
        bool removeItem(const std::shared_ptr<T> &widget)
        { return _tgui.remove(widget); }

        template<typename T, typename = std::enable_if_t<std::is_base_of<tgui::Widget, T>::value>>
        typename T::Ptr get(const std::string &name, bool recursive = false) const
        { return _tgui.get(name, recursive); }

        tgui::Gui &getGui() noexcept { return _tgui; }
    private:
        tgui::Gui _tgui;
#endif

        // Coplien
    public:
        Engine(const Engine &) = delete;

        Engine &operator=(const Engine &) = delete;
    };
}
