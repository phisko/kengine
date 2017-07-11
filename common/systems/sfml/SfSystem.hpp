#pragma once

#include "System.hpp"
#include "common/components/MetaComponent.hpp"
#include "common/packets/RegisterAppearance.hpp"

#include "pse/Engine.hpp"

namespace kengine
{
    class EntityManager;

    class SfSystem : public kengine::System<SfSystem,
                                            packets::RegisterAppearance>
    {
    public:
        SfSystem(kengine::EntityManager& em);

    public:
        void execute() final;
        void registerGameObject(kengine::GameObject& go) override;
        void removeGameObject(kengine::GameObject& go) override;

    public:
        void handle(const packets::RegisterAppearance& p) noexcept;

    private:
        std::unique_ptr<pse::ViewItem> getResource(const kengine::GameObject& go);

    private:
        putils::json::Object       _config;
        putils::Point<std::size_t> _screenSize;
        putils::Point<std::size_t> _tileSize;
        bool                       _fullScreen;

        // Config parsers
    private:
        putils::Point<std::size_t> parseSize(std::string_view propertyName,
                                             const putils::Point<std::size_t>& _default);
        bool parseBool(std::string_view propertyName, bool _default);

    private:
        kengine::EntityManager                                                   & _em;
        pse::Engine                                                              _engine;
        std::unordered_map<kengine::GameObject*, std::unique_ptr<pse::ViewItem>> _viewItems;
        std::unordered_map<std::string, std::string>                             _appearances;
    };
}
