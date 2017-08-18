#include "SfSystem.hpp"
#include "SfComponent.hpp"

#include "EntityManager.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Log.hpp"

#include "pluginManager/Export.hpp"

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
    {}

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
     * Helper
     */

    SfComponent& SfSystem::getResource(kengine::GameObject& go)
    {
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

    /*
     * System functions
     */

    void SfSystem::execute()
    {
        for (auto go : _em.getGameObjects<SfComponent>())
        {
            auto      & comp      = go->getComponent<SfComponent>();
            const auto& transform = go->getComponent<kengine::TransformComponent3d>();
            const auto& pos       = transform.boundingBox.topLeft;
            const auto& size      = transform.boundingBox.size;

            comp.getViewItem().setPosition(
                    {(float) (_tileSize.x * pos.x), (float) (_tileSize.y * pos.y)});
            if (!comp.isFixedSize())
                comp.getViewItem().setSize(
                        {(float) (_tileSize.x * size.x), (float) (_tileSize.y * size.y)});

            _engine.setItemHeight(comp.getViewItem(), (std::size_t) pos.z);
        }

        sf::Event e;
        while (_engine.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                getMediator()->running = false;
            else if (e.type == sf::Event::KeyPressed)
            {
                const auto it = _keyHandlers.find(e.key.code);
                if (it != _keyHandlers.end())
                    it->second.onPress();
            }
            else if (e.type == sf::Event::KeyReleased)
            {
                const auto it = _keyHandlers.find(e.key.code);
                if (it != _keyHandlers.end())
                    it->second.onRelease();
            }
            else if (e.type == sf::Event::MouseMoved && _mouseMovedHandler != nullptr)
                _mouseMovedHandler({ e.mouseMove.x, e.mouseMove.y });
            else if (e.type == sf::Event::MouseButtonPressed)
            {
                const auto it = _mouseButtonHandlers.find(e.mouseButton.button);
                if (it != _mouseButtonHandlers.end())
                    it->second.onPress();
            }
            else if (e.type == sf::Event::MouseButtonReleased)
            {
                const auto it = _mouseButtonHandlers.find(e.mouseButton.button);
                if (it != _mouseButtonHandlers.end())
                    it->second.onRelease();
            }
        }

        _engine.update(true);
    }

    void SfSystem::registerGameObject(kengine::GameObject& go)
    {
        if (!go.hasComponent<SfComponent>() && !go.hasComponent<MetaComponent>())
            return;

        try
        {
            auto& v = go.hasComponent<SfComponent>() ? go.getComponent<SfComponent>()
                                                     : getResource(go);

            const auto& transform = go.getComponent<kengine::TransformComponent3d>();

            const auto& pos = transform.boundingBox.topLeft;
            v.getViewItem().setPosition({(float) (_tileSize.x * pos.x), (float) (_tileSize.y *
                                                                                 pos.y)});

            if (!v.isFixedSize())
            {
                const auto& size = transform.boundingBox.size;
                v.getViewItem().setSize({(float) (_tileSize.x * size.x), (float) (_tileSize.y *
                                                                                  size.y)});
            }

            _engine.addItem(v.getViewItem(), (std::size_t) pos.z);
        }
        catch (const std::exception &e)
        {
            send(kengine::packets::Log{
                    putils::concat("[SfSystem] Unknown appearance: ",
                                   go.getComponent<MetaComponent>().appearance)
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
}
