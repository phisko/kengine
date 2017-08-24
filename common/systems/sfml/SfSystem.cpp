#include "SfSystem.hpp"
#include "SfComponent.hpp"

#include "EntityManager.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Log.hpp"

#include "pluginManager/Export.hpp"

#include "common/systems/LuaSystem.hpp"
#include "common/components/GUIComponent.hpp"

EXPORT kengine::ISystem* getSystem(kengine::EntityManager& em)
{
    return new kengine::SfSystem(em);
}

namespace kengine
{
    /*
     * Constructor
     */

    static putils::json::Object parseConfig()
    {
        std::ifstream config("sf-config.json");
        std::string   str((std::istreambuf_iterator<char>(config)),
                          std::istreambuf_iterator<char>());
        return putils::json::lex(str);
    }

    SfSystem::SfSystem(kengine::EntityManager& em)
            : _config(parseConfig()),
              _screenSize(parseSize("windowSize", {1280, 720})),
              _tileSize(parseSize("tileSize", {1, 1})),
              _fullScreen(parseBool("fullScreen", false)),
              _em(em),
              _engine(_screenSize.x, _screenSize.y, "Kengine",
                      _fullScreen ? sf::Style::Fullscreen : sf::Style::Close)
    {
        try
        {
            auto &lua = em.getSystem<kengine::LuaSystem>().getState();

            lua["getWindowSize"] =
                    [this]
                    {
                        const auto size = _engine.getRenderWindow().getSize();
                        return putils::Point3d{ (double)size.x, (double)size.y };
                    };

            lua["getTileSize"] =
                    [this] { return putils::Point3d{ (double)_tileSize.x, (double)_tileSize.y }; };

            lua["setKeyHandler"] =
                    [this](const std::function<void(sf::Keyboard::Key)> &onPress,
                                const std::function<void(sf::Keyboard::Key)> &onRelease)
                    {
                        auto &handler = _keyHandlers[sf::Keyboard::KeyCount];
                        handler.onPress = onPress;
                        handler.onRelease = onRelease;
                    };

            lua["setMouseButtonHandler"] =
                    [this, &em](const std::function<void(sf::Mouse::Button, int x, int y)> &onPress,
                                const std::function<void(sf::Mouse::Button, int x, int y)> &onRelease)
                    {
                        auto &handler = _mouseButtonHandlers[sf::Mouse::ButtonCount];
                        handler.onPress = onPress;
                        handler.onRelease = onRelease;
                    };

            lua["setMouseMovedHandler"] =
                    [this, &em](const std::function<void(int x, int y)> &func)
                    { _mouseMovedHandler = [func](const putils::Point2i &p) { func(p.x, p.y); }; };
        }
        catch (const std::out_of_range &) {}
    }

    /*
     * Config parsers
     */

    putils::Point<std::size_t> SfSystem::parseSize(std::string_view jsonProperty,
                                                   const putils::Point<std::size_t>& _default)
    {
        if (_config.fields.find(jsonProperty.data()) != _config.fields.end())
            return {
                    (std::size_t) std::stoi(_config[jsonProperty]["x"]),
                    (std::size_t) std::stoi(_config[jsonProperty]["y"])
            };

        return _default;
    }

    bool SfSystem::parseBool(std::string_view propertyName, bool _default)
    {
        if (_config.fields.find(propertyName.data()) != _config.fields.end())
            return _config[propertyName].value == "true";
        return _default;
    }

    /*
     * System functions
     */

    void SfSystem::execute()
    {
        // Update positions
        for (auto go : _em.getGameObjects<SfComponent>())
        {
            auto &comp = go->getComponent<SfComponent>();
            const auto &transform = go->getComponent<kengine::TransformComponent3d>();
            const auto &pos = transform.boundingBox.topLeft;

            comp.getViewItem().setPosition(
                    {(float) (_tileSize.x * pos.x), (float) (_tileSize.y * pos.y)});

            const auto &size = transform.boundingBox.size;
            if (!comp.isFixedSize())
                comp.getViewItem().setSize(
                        {(float) (_tileSize.x * size.x), (float) (_tileSize.y * size.y)});

            _engine.setItemHeight(comp.getViewItem(), (std::size_t) pos.z);
        }

        // GUI elements
        for (const auto go : _em.getGameObjects<kengine::GUIComponent>())
        {
            const auto &gui = go->getComponent<kengine::GUIComponent>();
            auto &view = static_cast<pse::Text &>(go->getComponent<SfComponent>().getViewItem());
            view.setString(gui.text);
        }

        // Event handling
        sf::Event e;
        while (_engine.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                getMediator()->running = false;
                _engine.getRenderWindow().close();
                return;
            }
            else if (e.type == sf::Event::KeyPressed)
            {
                const auto it = _keyHandlers.find(e.key.code);
                if (it != _keyHandlers.end())
                    it->second.onPress(e.key.code);

                const auto it2 = _keyHandlers.find(sf::Keyboard::KeyCount);
                if (it2 != _keyHandlers.end())
                    it2->second.onPress(e.key.code);
            }
            else if (e.type == sf::Event::KeyReleased)
            {
                const auto it = _keyHandlers.find(e.key.code);
                if (it != _keyHandlers.end())
                    it->second.onRelease(e.key.code);

                const auto it2 = _keyHandlers.find(sf::Keyboard::KeyCount);
                if (it2 != _keyHandlers.end())
                    it2->second.onRelease(e.key.code);
            }
            else if (e.type == sf::Event::MouseMoved && _mouseMovedHandler != nullptr)
                _mouseMovedHandler({ e.mouseMove.x, e.mouseMove.y });
            else if (e.type == sf::Event::MouseButtonPressed)
            {
                const auto it = _mouseButtonHandlers.find(e.mouseButton.button);
                if (it != _mouseButtonHandlers.end())
                    it->second.onPress(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);

                const auto it2 = _mouseButtonHandlers.find(sf::Mouse::ButtonCount);
                if (it2 != _mouseButtonHandlers.end())
                    it2->second.onPress(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
            }
            else if (e.type == sf::Event::MouseButtonReleased)
            {
                const auto it = _mouseButtonHandlers.find(e.mouseButton.button);
                if (it != _mouseButtonHandlers.end())
                    it->second.onRelease(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);

                const auto it2 = _mouseButtonHandlers.find(sf::Mouse::ButtonCount);
                if (it2 != _mouseButtonHandlers.end())
                    it2->second.onRelease(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
            }
        }

        _engine.update(true);
    }

    void SfSystem::registerGameObject(kengine::GameObject& go)
    {
        if (!go.hasComponent<SfComponent>() && !go.hasComponent<MetaComponent>() && !go.hasComponent<kengine::GUIComponent>())
            return;

        try
        {
            auto &v = go.hasComponent<SfComponent>() ? go.getComponent<SfComponent>()
                                                     : getResource(go);

            const auto &transform = go.getComponent<kengine::TransformComponent3d>();

            const auto &pos = transform.boundingBox.topLeft;
            v.getViewItem().setPosition(
                    { (float) (_tileSize.x * pos.x), (float) (_tileSize.y * pos.y) }
            );

            if (!v.isFixedSize())
            {
                const auto &size = transform.boundingBox.size;
                v.getViewItem().setSize(
                        { (float) (_tileSize.x * size.x), (float) (_tileSize.y * size.y) }
                );
            }

            _engine.addItem(v.getViewItem(), (std::size_t) pos.z);
        }
        catch (const std::exception &e)
        {
            send(kengine::packets::Log{
                    putils::concat("[SfSystem] Unknown appearance: ", go.getComponent<MetaComponent>().appearance)
            });
        }
    }

    void SfSystem::removeGameObject(kengine::GameObject& go)
    {
        if (!go.hasComponent<SfComponent>())
            return;

        const auto& comp = go.getComponent<SfComponent>();
        _engine.removeItem(comp.getViewItem());
        _em.detachComponent(go, comp);
    }

    /*
     * DataPacket handlers
     */

    void SfSystem::handle(const kengine::packets::RegisterAppearance& p) noexcept
    {
        _appearances[p.appearance] = p.resource;
    }

    void SfSystem::handle(const packets::RegisterKeyHandler &p) noexcept
    {
        _keyHandlers[p.key] = p;
    }

    void SfSystem::handle(const packets::RegisterMouseMovedHandler &p) noexcept
    {
        _mouseMovedHandler = p.handler;
    }

    void SfSystem::handle(const packets::RegisterMouseButtonHandler &p) noexcept
    {
        _mouseButtonHandlers[p.button] = p;
    }

    void SfSystem::handle(const packets::KeyStatus::Query &p) noexcept
    {
        sendTo(packets::KeyStatus::Response { sf::Keyboard::isKeyPressed(p.key) }, *p.sender);
    }

    void SfSystem::handle(const packets::MouseButtonStatus::Query &p) noexcept
    {
        sendTo(packets::MouseButtonStatus::Response { sf::Mouse::isButtonPressed(p.button) }, *p.sender);
    }

    void SfSystem::handle(const packets::MousePosition::Query &p) noexcept
    {
        const auto pos = sf::Mouse::getPosition();
        sendTo(packets::MousePosition::Response { { pos.x, pos.y } }, *p.sender);
    }

    /*
     * Helper
     */

    SfComponent& SfSystem::getResource(kengine::GameObject& go)
    {
        if (go.hasComponent<GUIComponent>())
        {
            const auto &gui = go.getComponent<GUIComponent>();
            auto &comp = go.attachComponent<SfComponent>(
                    gui.text, sf::Vector2f{ 0, 0 }, sf::Color::White, gui.textSize, gui.font
            );
            return comp;
        }

        const auto& meta = go.getComponent<MetaComponent>();

        std::string_view str = _appearances.find(meta.appearance) != _appearances.end()
                               ? _appearances.at(meta.appearance)
                               : meta.appearance;

        auto& comp = go.attachComponent<SfComponent>(
                std::make_unique<pse::Sprite>(str.data(),
                                              sf::Vector2f{0, 0},
                                              sf::Vector2f{16, 16})
        );

        return comp;
    }
}

