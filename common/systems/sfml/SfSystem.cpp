#include "SfSystem.hpp"

#include "EntityManager.hpp"
#include "components/TransformComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/GUIComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "packets/Log.hpp"
#include "packets/LuaState.hpp"
#include "lua/plua.hpp"

#include "imgui.h"
#include "imgui-SFML.h"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
    return new kengine::SfSystem(em);
}

namespace kengine {
    /*
     * Constructor
     */

    static putils::json::Object parseConfig() {
        std::ifstream config("sf-config.json");
        std::string str((std::istreambuf_iterator<char>(config)),
                        std::istreambuf_iterator<char>());
        return putils::json::lex(str);
    }

    SfSystem::SfSystem(kengine::EntityManager & em)
            : putils::BaseModule(&em),
              _config(parseConfig()),
              _screenSize(parseSize("windowSize", { 1280, 720 })),
              _tileSize(parseSize("tileSize", { 1, 1 })),
              _fullScreen(parseBool("fullScreen", false)),
              _em(em),
              _engine(_screenSize.x, _screenSize.y, "Kengine",
                      _fullScreen ? sf::Style::Fullscreen : sf::Style::Close) {
        for (const auto go : _em.getGameObjects())
            handle(kengine::packets::RegisterGameObject{ *go });

        registerLuaFunctions();

        ImGui::SFML::Init(_engine.getRenderWindow());
    }

    void SfSystem::registerLuaFunctions() noexcept {
        try {
            auto & lua = *query<kengine::packets::LuaState::Response>(kengine::packets::LuaState::Query{}).state;

            lua["getWindowSize"] = [this] {
                const auto size = _engine.getRenderWindow().getSize();
                return putils::Point3d{ (double) size.x, (double) size.y };
            };

            lua["getTileSize"] = [this] {
                return putils::Point3d{ (double) _tileSize.x, (double) _tileSize.y };
            };

            lua["getGridSize"] = [this] {
                const auto size = _engine.getRenderWindow().getSize();
                return putils::Point3d{ (double) (size.x / _tileSize.x), double(size.y / _tileSize.y) };
            };
        }
        catch (const std::out_of_range &) {}
    }

    /*
     * Config parsers
     */

    putils::Point<std::size_t> SfSystem::parseSize(const std::string & jsonProperty, const putils::Point<std::size_t> & _default) {
        if (_config.fields.find(jsonProperty) != _config.fields.end())
            return {
                    (std::size_t) std::stoi(_config[jsonProperty]["x"]),
                    (std::size_t) std::stoi(_config[jsonProperty]["y"])
            };

        return _default;
    }

    bool SfSystem::parseBool(const std::string & propertyName, bool _default) {
        if (_config.fields.find(propertyName) != _config.fields.end())
            return _config[propertyName].value == "true";
        return _default;
    }

    /*
     * System functions
     */

    void SfSystem::execute() {
        handleEvents();

        ImGui::SFML::Update(_engine.getRenderWindow(), _deltaClock.restart());

		const auto & objects = _em.getGameObjects<kengine::ImGuiComponent>();
		for (const auto go : objects) {
			const auto & comp = go->getComponent<kengine::ImGuiComponent>();
			comp.display(GImGui);
		}

        updateCameras();
        updateDrawables();

        _engine.update(true, [this] { ImGui::SFML::Render(_engine.getRenderWindow()); });
    }

    void SfSystem::updateCameras() noexcept {
        const auto & cameras = _em.getGameObjects<kengine::CameraComponent3d>();

        if (!cameras.empty())
            _engine.removeView("default");

        for (auto go : cameras) {
            auto & view = _engine.getView(go->getName());

            const auto & frustrum = go->getComponent<kengine::CameraComponent3d>().frustrum;
            view.setCenter(
                    (float)(frustrum.topLeft.x + frustrum.size.x / 2) * _tileSize.x,
                    (float)(frustrum.topLeft.z + frustrum.size.z / 2) * _tileSize.y
            );
            view.setSize((float)frustrum.size.x * _tileSize.x, (float)frustrum.size.z * _tileSize.y);

            const auto & box = go->getComponent<kengine::TransformComponent3d>().boundingBox;
            view.setViewport(sf::FloatRect{
                    (float)box.topLeft.x, (float)box.topLeft.z,
                    (float)box.size.x, (float)box.size.z
            });
            _engine.setViewHeight(go->getName(), (size_t)box.topLeft.y);
        }
    }

    void SfSystem::updateDrawables() noexcept {
        std::vector<kengine::GameObject *> toDetach;

        for (auto go : _em.getGameObjects<SfComponent>()) {
            auto & comp = go->getComponent<SfComponent>();

            if (go->hasComponent<kengine::GUIComponent>())
                updateGUIElement(*go, comp);
            else if (go->hasComponent<kengine::GraphicsComponent>())
                updateObject(*go, comp);
            else
                toDetach.push_back(go);
        }

        for (const auto go : toDetach)
            go->detachComponent<SfComponent>();
    }

    void SfSystem::updateObject(kengine::GameObject & go, SfComponent & comp) noexcept {
        const auto & transform = go.getComponent<kengine::TransformComponent3d>();
        updateTransform(go, comp, transform);

        const auto & graphics = go.getComponent<kengine::GraphicsComponent>();
        const auto & appearance = graphics.appearance;
        auto & sprite = static_cast<pse::Sprite &>(comp.getViewItem());
        sprite.setTexture(appearance);

        sprite.setRotation(-transform.yaw - graphics.yaw);

        if (graphics.size.x != 0 || graphics.size.z != 0) {
            sprite.setSize(
                    { (float) (_tileSize.x * graphics.size.x), (float) (_tileSize.y * graphics.size.z) }
            );
        }

        if (graphics.repeated) {
            const auto & box = transform.boundingBox;

            sf::IntRect rect = (graphics.size.x != 0 || graphics.size.z != 0) ? sf::IntRect{
                    (int)(_tileSize.x * box.topLeft.x), (int)(_tileSize.y * box.topLeft.z),
                    (int)(_tileSize.x * graphics.size.x), (int)(_tileSize.x * graphics.size.z)
            } : sf::IntRect{
                    (int)(_tileSize.x * box.topLeft.x), (int)(_tileSize.y * box.topLeft.z),
                    (int)(_tileSize.x * box.size.x), (int)(_tileSize.x * box.size.z)
            };

            sprite.repeat(rect);
        } else
            sprite.unrepeat();
    }

    void SfSystem::updateGUIElement(kengine::GameObject & go, SfComponent & comp) noexcept {
        const auto & gui = go.getComponent<kengine::GUIComponent>();
        auto & view = static_cast<pse::Text &>(comp.getViewItem());
        view.setString(gui.text);

        auto & transform = go.getComponent<kengine::TransformComponent3d>();
        if (gui.camera != nullptr) {
            const auto & frustrum = gui.camera->getComponent<kengine::CameraComponent3d>().frustrum;
            transform.boundingBox.topLeft.x = frustrum.topLeft.x + frustrum.size.x * gui.topLeft.x;
            transform.boundingBox.topLeft.z = frustrum.topLeft.z + frustrum.size.z * gui.topLeft.z;
            transform.boundingBox.topLeft.y = gui.topLeft.y;
        }

        updateTransform(go, comp, transform);
    }

    void SfSystem::updateTransform(kengine::GameObject & go, SfComponent & comp, const kengine::TransformComponent3d & transform) noexcept {
        const auto & pos = transform.boundingBox.topLeft;
        comp.getViewItem().setPosition(
                { (float) (_tileSize.x * pos.x), (float) (_tileSize.y * pos.z) }
        );
        _engine.setItemHeight(comp.getViewItem(), (std::size_t) pos.y);

        const auto & size = transform.boundingBox.size;
        if (!comp.isFixedSize())
            comp.getViewItem().setSize(
                    { (float) (_tileSize.x * size.x), (float) (_tileSize.y * size.z) }
            );

        comp.getViewItem().setRotation(-transform.yaw);

    }

    void SfSystem::handleEvents() noexcept {
        sf::Event e;
        std::vector<sf::Event> allEvents;
        while (_engine.pollEvent(e)) {
			ImGui::SFML::ProcessEvent(e);

            if (e.type == sf::Event::Closed) {
                getMediator()->running = false;
                _engine.getRenderWindow().close();
                return;
            }
            allEvents.push_back(e);
        }

        for (const auto go : _em.getGameObjects<kengine::InputComponent>()) {
            const auto & input = go->getComponent<kengine::InputComponent>();
            for (const auto & e : allEvents) {
                if (input.onMouseButton != nullptr && (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::MouseButtonPressed))
                    input.onMouseButton(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y, e.type == sf::Event::MouseButtonPressed);
                else if (input.onMouseMove != nullptr && e.type == sf::Event::MouseMoved)
                    input.onMouseMove(e.mouseMove.x, e.mouseMove.y);
                else if (input.onKey != nullptr && (e.type == sf::Event::KeyPressed || e.type == sf::Event::KeyReleased))
                    input.onKey(e.key.code, e.type == sf::Event::KeyPressed);
            }
        }
    }

    void SfSystem::handle(const kengine::packets::RegisterGameObject & p) {
        auto & go = p.go;
        if (!go.hasComponent<SfComponent>() && !go.hasComponent<GraphicsComponent>() &&
            !go.hasComponent<kengine::GUIComponent>())
            return;

        try {
            auto & v = go.hasComponent<SfComponent>() ? go.getComponent<SfComponent>()
                                                      : getResource(go);

            const auto & transform = go.getComponent<kengine::TransformComponent3d>();

            const auto & pos = transform.boundingBox.topLeft;
            v.getViewItem().setPosition(
                    { (float) (_tileSize.x * pos.x), (float) (_tileSize.y * pos.z) }
            );

            if (!v.isFixedSize()) {
                const auto & size = transform.boundingBox.size;
                v.getViewItem().setSize(
                        { (float) (_tileSize.x * size.x), (float) (_tileSize.y * size.z) }
                );
            }

            _engine.addItem(v.getViewItem(), (std::size_t) pos.y);
        }
        catch (const std::exception & e) {
            send(kengine::packets::Log{
                    putils::concat("[SfSystem] Unknown appearance: ", go.getComponent<GraphicsComponent>().appearance)
            });
        }
    }

    void SfSystem::handle(const kengine::packets::RemoveGameObject & p) {
        auto & go = p.go;

        if (go.hasComponent<kengine::CameraComponent3d>() && _engine.hasView(go.getName()))
            _engine.removeView(go.getName());

        if (!go.hasComponent<SfComponent>())
            return;

        const auto & comp = go.getComponent<SfComponent>();
        _engine.removeItem(comp.getViewItem());
    }

    /*
     * DataPacket handlers
     */

    void SfSystem::handle(const kengine::packets::RegisterAppearance & p) noexcept {
        _appearances[p.appearance] = p.resource;
    }

    void SfSystem::handle(const packets::KeyStatus::Query & p) const noexcept {
        sendTo(packets::KeyStatus::Response { sf::Keyboard::isKeyPressed(p.key) }, *p.sender);
    }

    void SfSystem::handle(const packets::MouseButtonStatus::Query & p) const noexcept {
        sendTo(packets::MouseButtonStatus::Response { sf::Mouse::isButtonPressed(p.button) }, *p.sender);
    }

    void SfSystem::handle(const packets::MousePosition::Query & p) const noexcept {
        const auto pos = sf::Mouse::getPosition();
        sendTo(packets::MousePosition::Response { { pos.x, pos.y } }, *p.sender);
    }

    /*
     * Helper
     */

    SfComponent & SfSystem::getResource(kengine::GameObject & go) {
        if (go.hasComponent<GUIComponent>()) {
            const auto & gui = go.getComponent<GUIComponent>();
            auto & comp = go.attachComponent<SfComponent>(
                    gui.text, sf::Vector2f{ 0, 0 }, sf::Color::White, gui.textSize, gui.font
            );
            return comp;
        }

        const auto & meta = go.getComponent<GraphicsComponent>();

        const auto & str = _appearances.find(meta.appearance) != _appearances.end()
                           ? _appearances.at(meta.appearance)
                           : meta.appearance;

        auto & comp = go.attachComponent<SfComponent>(
                std::make_unique<pse::Sprite>(str, sf::Vector2f{ 0, 0 }, sf::Vector2f{ 16, 16 })
        );

        return comp;
    }
}

