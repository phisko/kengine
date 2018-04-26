#pragma once

#include "System.hpp"
#include "components/TransformComponent.hpp"
#include "packets/RegisterAppearance.hpp"
#include "packets/Input.hpp"
#include "packets/RemoveGameObject.hpp"
#include "packets/RegisterGameObject.hpp"
#include "packets/ScreenSize.hpp"

#include "pse/Engine.hpp"
#include "SfComponent.hpp"

namespace kengine {
    class EntityManager;

    class SfSystem : public kengine::System<SfSystem,
            packets::RegisterGameObject, packets::RemoveGameObject,
            packets::RegisterAppearance,
			packets::ScreenSize::GridSizeQuery, packets::ScreenSize::TileSizeQuery, packets::ScreenSize::ScreenSizeQuery,
            packets::KeyStatus::Query, packets::MouseButtonStatus::Query, packets::MousePosition::Query> {
    public:
        SfSystem(kengine::EntityManager & em);

    public:
        void execute() final;
        void handle(const kengine::packets::RegisterGameObject & p);
        void handle(const kengine::packets::RemoveGameObject & p);

    public:
        void handle(const packets::RegisterAppearance & p) noexcept;

    public:
		void handle(const packets::ScreenSize::GridSizeQuery & p) const noexcept { sendTo(packets::ScreenSize::Response{ putils::Point2d{ _screenSize.x / _tileSize.x, _screenSize.y / _tileSize.y } }, *p.sender); }
		void handle(const packets::ScreenSize::TileSizeQuery & p) const noexcept { sendTo(packets::ScreenSize::Response{ _tileSize }, *p.sender); }
		void handle(const packets::ScreenSize::ScreenSizeQuery & p) const noexcept { sendTo(packets::ScreenSize::Response{ _screenSize }, *p.sender); }

    public:
        void handle(const packets::KeyStatus::Query & p) const noexcept;
        void handle(const packets::MouseButtonStatus::Query & p) const noexcept;
        void handle(const packets::MousePosition::Query & p) noexcept;

    private:
        SfComponent & getResource(kengine::GameObject & go);
        void registerLuaFunctions() noexcept;
        void handleEvents() noexcept;
        void updateCameras() noexcept;
        void updateDrawables();
        void updateObject(kengine::GameObject & go, SfComponent & comp);
        void updateGUIElement(kengine::GameObject & go, SfComponent & comp) noexcept;
        void updateTransform(kengine::GameObject & go, SfComponent & comp, const kengine::TransformComponent3d & transform) noexcept;

	private:
		putils::json::Object _config;
		putils::Point2d _screenSize;
		putils::Point2d _tileSize;
		bool _fullScreen;

		// Config parsers
	private:
		putils::Point2d parseSize(const std::string & propertyName, const putils::Point2d & _default);
		bool parseBool(const std::string & propertyName, bool _default);

	private:
		kengine::EntityManager & _em;
		pse::Engine _engine;

	private:
		sf::Clock _deltaClock;

	private:
		std::unordered_map<std::string, std::string> _appearances;

		// Input
	private:
		std::unordered_map<sf::Keyboard::Key, bool> _pressedKeys;
		std::unordered_map<sf::Mouse::Button, bool> _pressedButtons;
	};
}
