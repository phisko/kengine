#pragma once

#include <SDL2/SDL.h>
#include "System.hpp"
#include "common/components/MetaComponent.hpp"
#include "common/packets/RegisterAppearance.hpp"
#include "common/packets/Input.hpp"

#include "pse/Engine.hpp"
#include "SfComponent.hpp"

namespace kengine
{
    class EntityManager;

    class SfSystem : public kengine::System<SfSystem, packets::RegisterAppearance,
            packets::RegisterKeyHandler, packets::RegisterMouseButtonHandler, packets::RegisterMouseMovedHandler,
            packets::KeyStatus::Query, packets::MouseButtonStatus::Query, packets::MousePosition::Query>
    {
    public:
        SfSystem(kengine::EntityManager& em);

    public:
        void execute() final;
        void registerGameObject(kengine::GameObject& go) override;
        void removeGameObject(kengine::GameObject& go) override;

    public:
        void handle(const packets::RegisterAppearance& p) noexcept;

    public:
        void handle(const packets::RegisterKeyHandler &p) noexcept;
        void handle(const packets::RegisterMouseMovedHandler &p) noexcept;
        void handle(const packets::RegisterMouseButtonHandler &p) noexcept;

    public:
        void handle(const packets::KeyStatus::Query &p) noexcept;
        void handle(const packets::MouseButtonStatus::Query &p) noexcept;
        void handle(const packets::MousePosition::Query &p) noexcept;

    private:
        SfComponent &getResource(kengine::GameObject& go);

    private:
        putils::json::Object _config;
        putils::Point<std::size_t> _screenSize;
        putils::Point<std::size_t> _tileSize;
        bool _fullScreen;

        // Config parsers
    private:
        putils::Point<std::size_t> parseSize(std::string_view propertyName,
                                             const putils::Point<std::size_t>& _default);
        bool parseBool(std::string_view propertyName, bool _default);

    private:
        kengine::EntityManager & _em;
        pse::Engine _engine;
        std::unordered_map<kengine::GameObject*, std::unique_ptr<pse::ViewItem>> _viewItems;
        std::unordered_map<std::string, std::string> _appearances;

        // Input
    private:
        std::unordered_map<sf::Keyboard::Key, packets::RegisterKeyHandler> _keyHandlers;
        std::function<void(const putils::Point2i &)> _mouseMovedHandler = nullptr;
        std::unordered_map<sf::Mouse::Button, packets::RegisterMouseButtonHandler> _mouseButtonHandlers;
    };
}
