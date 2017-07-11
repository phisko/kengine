#include <pse/Shape.hpp>
#include "SfSystem.hpp"
#include "SfComponent.hpp"
#include "pluginManager/Export.hpp"

#include "EntityManager.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Log.hpp"

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
              _engine(_screenSize.x, _screenSize.y, "Koadom Wars",
                      _fullScreen ? sf::Style::Fullscreen : sf::Style::Close)
    {
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
 * Helper
 */

    static std::unique_ptr<pse::Sprite> makeSprite(std::string_view sprite)
    {
        return std::make_unique<pse::Sprite>(sprite.data(), sf::Vector2f{0, 0},
                                             sf::Vector2f{16, 16});
    }

    std::unique_ptr<pse::ViewItem> SfSystem::getResource(const kengine::GameObject& go)
    {
        const auto& meta = go.getComponent<MetaComponent>();

        std::string_view str = _appearances.find(meta.appearance) != _appearances.end()
                               ? _appearances.at(meta.appearance)
                               : meta.appearance;

        return makeSprite(str);
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
            comp.getViewItem().setSize(
                    {(float) (_tileSize.x * size.x), (float) (_tileSize.y * size.y)});

            _engine.setItemHeight(comp.getViewItem(), (std::size_t) pos.z);
        }

        sf::Event e;
        while (_engine.pollEvent(e))
            if (e.type == sf::Event::Closed)
                getMediator()->running = false;

        _engine.update(true);
    }

    void SfSystem::registerGameObject(kengine::GameObject& go)
    {
        if (go.hasComponent<SfComponent>() || !go.hasComponent<MetaComponent>())
            return;

        try
        {
            auto v = getResource(go);
            const auto& transform = go.getComponent<kengine::TransformComponent3d>();

            const auto& pos = transform.boundingBox.topLeft;
            v->setPosition({(float) (_tileSize.x * pos.x), (float) (_tileSize.y * pos.y)});

            const auto& size = transform.boundingBox.size;
            v->setSize({(float) (_tileSize.x * size.x), (float) (_tileSize.y * size.y)});

            auto& viewItem = _em.attachComponent<SfComponent>(go, std::move(v));

            _engine.addItem(viewItem.getViewItem(), (std::size_t) pos.z);
        }
        catch (const std::out_of_range& e)
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
}
