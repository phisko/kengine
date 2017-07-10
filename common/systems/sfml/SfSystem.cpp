#include <pse/Shape.hpp>
#include "SfSystem.hpp"
#include "SfComponent.hpp"
#include "pluginManager/Export.hpp"

#include "EntityManager.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Log.hpp"

EXPORT kengine::ISystem *getSystem(kengine::EntityManager &em)
{
    return new kengine::SfSystem(em);
}

static const putils::Point<std::size_t> screenSize { 1280, 720 };

static const putils::Point<std::size_t> tileSize { 32, 32 };

static const putils::Point<std::size_t> tiles
        {
                screenSize.x / tileSize.x,
                screenSize.y / tileSize.y
        };

namespace kengine
{
/*
 * Constructor
 */

    SfSystem::SfSystem(kengine::EntityManager &em)
            : _em(em),
              _engine(screenSize.x, screenSize.y, "Koadom Wars")
    {
    }

/*
 * Helper
 */

    static std::unique_ptr<pse::Sprite> makeSprite(std::string_view sprite)
    {
        return std::make_unique<pse::Sprite>(sprite.data(), sf::Vector2f{0, 0}, sf::Vector2f{16, 16});
    }

    std::unique_ptr<pse::ViewItem> SfSystem::getResource(const kengine::GameObject &go)
    {
        const auto &meta = go.getComponent<MetaComponent>();

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
        for (auto go : getGameObjects())
        {
            auto &comp = go->getComponent<SfComponent>();
            const auto &transform = go->getComponent<kengine::TransformComponent2d>();
            const auto &pos = transform.boundingBox.topLeft;
            const auto &size = transform.boundingBox.size;
            comp.getViewItem()->setPosition({ (float)(tileSize.x * pos.x), (float)(tileSize.y * pos.y)} );
            comp.getViewItem()->setSize({ (float)(tileSize.x * size.x), (float)(tileSize.y * size.y) });
        }

        sf::Event e;
        while (_engine.pollEvent(e))
            if (e.type == sf::Event::Closed)
                getMediator()->running = false;

        _engine.update(true);
    }

    void SfSystem::registerGameObject(kengine::GameObject &go)
    {
        if (go.hasComponent<SfComponent>() || !go.hasComponent<MetaComponent>())
            return;

        try
        {
            auto v = getResource(go);
            const auto &transform = go.getComponent<kengine::TransformComponent2d>();

            const auto &pos = transform.boundingBox.topLeft;
            v->setPosition({ (float)(tileSize.x * pos.x), (float)(tileSize.y * pos.y) });

            const auto &size = transform.boundingBox.size;
            v->setSize({ (float)(tileSize.x * size.x), (float)(tileSize.y * size.y) });

            const auto &viewItem = _em.attachComponent<SfComponent>(go, std::move(v));
            _engine.addItem(viewItem.getViewItem());
            getGameObjects().push_back(&go);
        }
        catch (const std::out_of_range &e)
        {
            send(kengine::packets::Log{
                    putils::concat("[SfSystem] Unknown appearance: ", go.getComponent<MetaComponent>().appearance)
            });
        }
    }

    void SfSystem::removeGameObject(kengine::GameObject &go)
    {
        if (!go.hasComponent<SfComponent>())
            return;

        const auto &comp = go.getComponent<SfComponent>();
        _engine.removeItem(comp.getViewItem());
        _em.detachComponent(go, comp);

        auto &gameObject = getGameObjects();
        gameObject.erase(std::find(gameObject.begin(), gameObject.end(), &go));
    }

/*
 * DataPacket handlers
 */

    void SfSystem::handle(const kengine::packets::RegisterAppearance &p) noexcept
    {
        _appearances[p.appearance] = p.resource;
    }
}
