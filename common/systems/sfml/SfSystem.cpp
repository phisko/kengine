#include "SfSystem.hpp"

#include "EntityManager.hpp"
#include "components/TransformComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/GUIComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/DebugGraphicsComponent.hpp"

#include "packets/Log.hpp"
#include "packets/LuaState.hpp"
#include "lua/plua.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "Shape.hpp"

// stolen from https://github.com/SFML/SFML/wiki/source:-line-segment-with-thickness
class sfLine : public sf::Shape
{
public:
	sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, float thickness, sf::Color color) {
		set(point1, point2, thickness, color);
	}

	void set(const sf::Vector2f & point1, const sf::Vector2f & point2, float thickness, sf::Color color) {
		const sf::Vector2f direction = point2 - point1;
		const sf::Vector2f unitDirection = direction / std::sqrt(direction.x*direction.x + direction.y*direction.y);
		const sf::Vector2f unitPerpendicular(-unitDirection.y, unitDirection.x);

		const sf::Vector2f offset = (thickness / 2.f)*unitPerpendicular;

		vertices[0] = point1 + offset;
		vertices[1] = point2 + offset;
		vertices[2] = point2 - offset;
		vertices[3] = point1 - offset;

		setFillColor(color);	

		update();
	}

	std::size_t getPointCount() const override { return 4; }
	sf::Vector2f getPoint(std::size_t index) const override { return vertices[index]; }

private:
	sf::Vector2f vertices[4];
};


EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new kengine::SfSystem(em);
}

namespace kengine {
	/*
	 * Constructor
	 */

	static sf::Vector2f convert(const putils::Point3d & pos) {
		return { (float)pos.x, (float)pos.z };
	}

	static putils::json parseConfig() {
		std::ifstream config("sf-config.json");
		std::string str((std::istreambuf_iterator<char>(config)),
			std::istreambuf_iterator<char>());
		return putils::json::parse(str);
	}

	SfSystem::SfSystem(kengine::EntityManager & em)
		: putils::BaseModule(&em),
		_config(parseConfig()),
		_screenSize(parseSize("windowSize", { 1280, 720 })),
		_tileSize(parseSize("tileSize", { 1, 1 })),
		_fullScreen(parseBool("fullScreen", false)),
		_em(em),
		_engine(_screenSize.x, _screenSize.y, "Kengine",
			_fullScreen ? sf::Style::Fullscreen : sf::Style::Default) {
		for (const auto go : _em.getGameObjects())
			handle(kengine::packets::RegisterGameObject{ *go });

		registerLuaFunctions();

		ImGui::SFML::Init(_engine.getRenderWindow());
	}

	void SfSystem::registerLuaFunctions() noexcept {
		try {
			const auto ptr = query<kengine::packets::LuaState::Response>(kengine::packets::LuaState::Query{}).state;
			if (ptr == nullptr)
				return;
			auto & lua = *ptr;

			lua["getWindowSize"] = [this] {
				const auto size = _engine.getRenderWindow().getSize();
				return putils::Point3d{ (double)size.x, (double)size.y };
			};

			lua["getTileSize"] = [this] {
				return putils::Point3d{ (double)_tileSize.x, (double)_tileSize.y };
			};

			lua["getGridSize"] = [this] {
				const auto size = _engine.getRenderWindow().getSize();
				return putils::Point3d{ (double)(size.x / _tileSize.x), double(size.y / _tileSize.y) };
			};
		}
		catch (const std::out_of_range &) {}
	}

	/*
	 * Config parsers
	 */

	putils::Point2d SfSystem::parseSize(const std::string & jsonProperty, const putils::Point2d & _default) {
		if (_config.find(jsonProperty) != _config.end())
			return {
					_config[jsonProperty]["x"],
					_config[jsonProperty]["y"]
		};

		return _default;
	}

	bool SfSystem::parseBool(const std::string & propertyName, bool _default) {
		if (_config.find(propertyName) != _config.end())
			return _config[propertyName];
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

		for (const auto go : cameras) {
			auto & view = _engine.getView(go->getName());

			const auto & frustrum = go->getComponent<kengine::CameraComponent3d>().frustrum;
			view.setCenter(toWorldPos(frustrum.getCenter()));
			view.setSize(toWorldPos(frustrum.size));

			const auto & box = go->getComponent<kengine::TransformComponent3d>().boundingBox;
			view.setViewport(sf::FloatRect{
					(float)box.topLeft.x, (float)box.topLeft.z,
					(float)box.size.x, (float)box.size.z
				});
			_engine.setViewHeight(go->getName(), (size_t)box.topLeft.y);
		}
	}

	void SfSystem::updateDrawables() {
		std::vector<kengine::GameObject *> toDetach;

		for (const auto go : _em.getGameObjects<SfComponent>()) {
			auto & comp = go->getComponent<SfComponent>();

			if (go->hasComponent<kengine::GUIComponent>())
				updateGUIElement(*go, comp);
			else if (go->hasComponent<kengine::GraphicsComponent>())
				updateObject(*go, comp);
			else if (go->hasComponent<kengine::DebugGraphicsComponent>())
				updateDebug(*go, comp);
			else
				toDetach.push_back(go);
		}

		for (const auto go : toDetach) {
			_engine.removeItem(go->getComponent<SfComponent>().getViewItem());
			go->detachComponent<SfComponent>();
		}
	}

	void SfSystem::updateDebug(kengine::GameObject & go, SfComponent & comp) {
		const auto & debug = go.getComponent<kengine::DebugGraphicsComponent>();
		auto & item = comp.getViewItem();
		if (debug.debugType == DebugGraphicsComponent::Text) {
			auto & text = static_cast<pse::Text &>(item);
			text.setString(debug.text);
			text.setTextSize(debug.textSize);
			text.setPosition(toWorldPos(debug.startPos));
			text.setColor(sf::Color(debug.color));
		}
		else if (debug.debugType == DebugGraphicsComponent::Line)
			static_cast<pse::Shape<sfLine> &>(item).get().set(
				toWorldPos(debug.startPos), toWorldPos(debug.endPos), debug.thickness, sf::Color(debug.color)
			);
		else if (debug.debugType == DebugGraphicsComponent::Sphere) {
			auto & circle = static_cast<pse::Shape<sf::CircleShape> &>(item).get();
			circle.setPosition(toWorldPos(debug.startPos));
			circle.setRadius(debug.radius * _tileSize.x);
			circle.setOrigin(circle.getRadius(), circle.getRadius());
			circle.setFillColor(sf::Color(debug.color));
		}
		_engine.setItemHeight(item, debug.startPos.y);
	}

	void SfSystem::updateObject(kengine::GameObject & go, SfComponent & comp) {
		const auto & transform = go.getComponent<kengine::TransformComponent3d>();
		updateTransform(go, comp, transform);

		const auto & graphics = go.getComponent<kengine::GraphicsComponent>();
		const auto & appearance = graphics.appearance;
		auto & sprite = static_cast<pse::Sprite &>(comp.getViewItem());

		try {
			sprite.setTexture(appearance);
		}
		catch (const std::exception & e) {
			std::cerr << "[SfSystem] Failed to set appearance: " << e.what() << std::endl;
		}

		sprite.setRotation(-transform.yaw - graphics.yaw);

		if (graphics.size.x != 0 || graphics.size.z != 0)
			sprite.setSize(toWorldPos(graphics.size));

		if (graphics.repeated) {
			const auto & box = transform.boundingBox;

			const sf::FloatRect rect(toWorldPos(box.topLeft), toWorldPos(graphics.size.x != 0 || graphics.size.z != 0 ? graphics.size : box.size));
			const sf::IntRect r(rect);

			sprite.repeat(r);
		}
		else
			sprite.unrepeat();
	}

	void SfSystem::updateGUIElement(kengine::GameObject & go, SfComponent & comp) noexcept {
		const auto & gui = go.getComponent<kengine::GUIComponent>();
		auto & view = static_cast<pse::Text &>(comp.getViewItem());
		view.setString(gui.text);
		view.setTextSize(gui.textSize);

		auto & transform = go.getComponent<kengine::TransformComponent3d>();
		if (!gui.camera.empty() && _em.hasEntity(gui.camera)) {
			const auto & cam = _em.getEntity(gui.camera);
			const auto & frustrum = cam.getComponent<kengine::CameraComponent3d>().frustrum;
			transform.boundingBox.topLeft.x = frustrum.topLeft.x + frustrum.size.x * gui.topLeft.x;
			transform.boundingBox.topLeft.z = frustrum.topLeft.z + frustrum.size.z * gui.topLeft.z;
			transform.boundingBox.topLeft.y = gui.topLeft.y;
		}

		updateTransform(go, comp, transform);
	}

	void SfSystem::updateTransform(kengine::GameObject & go, SfComponent & comp, const kengine::TransformComponent3d & transform) noexcept {
		const auto & pos = transform.boundingBox.topLeft;
		comp.getViewItem().setPosition(toWorldPos(pos));
		_engine.setItemHeight(comp.getViewItem(), (std::size_t) pos.y);

		const auto & size = transform.boundingBox.size;
		if (!comp.isFixedSize())
			comp.getViewItem().setSize(toWorldPos(size));

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

			const auto & io = ImGui::GetIO();
			if ((e.type == sf::Event::KeyPressed && !io.WantCaptureKeyboard) ||
				(e.type == sf::Event::KeyReleased && !io.WantCaptureKeyboard) ||
				(e.type == sf::Event::MouseMoved && !io.WantCaptureMouse) ||
				(e.type == sf::Event::MouseButtonPressed && !io.WantCaptureMouse) ||
				(e.type == sf::Event::MouseButtonReleased && !io.WantCaptureMouse) ||
				(e.type == sf::Event::MouseWheelScrolled && !io.WantCaptureMouse))
				allEvents.push_back(e);
		}

		const auto & objects = _em.getGameObjects<kengine::InputComponent>();
		for (const auto go : _em.getGameObjects<kengine::InputComponent>()) {
			const auto & input = go->getComponent<kengine::InputComponent>();
			for (const auto & e : allEvents) {
				if (input.onMouseButton != nullptr && (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::MouseButtonReleased))
					try {
					const auto x = (double)e.mouseButton.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
					const auto y = (double)e.mouseButton.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
					input.onMouseButton(e.mouseButton.button, x, y, e.type == sf::Event::MouseButtonPressed);
				}
				catch (const std::exception & e) {
					std::cerr << e.what() << std::endl;
				}
				else if (input.onMouseMove != nullptr && e.type == sf::Event::MouseMoved)
					try {
					const auto x = (double)e.mouseMove.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
					const auto y = (double)e.mouseMove.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
					input.onMouseMove(x, y);
				}
				catch (const std::exception & e) {
					std::cerr << e.what() << std::endl;
				}
				else if (input.onKey != nullptr && (e.type == sf::Event::KeyPressed || e.type == sf::Event::KeyReleased))
					try {
					input.onKey(e.key.code, e.type == sf::Event::KeyPressed);
				}
				catch (const std::exception & e) {
					std::cerr << e.what() << std::endl;
				}
			}
		}
	}

	void SfSystem::handle(const kengine::packets::RegisterGameObject & p) {
		auto & go = p.go;
		if (!go.hasComponent<SfComponent>() && !go.hasComponent<GraphicsComponent>() &&
			!go.hasComponent<kengine::GUIComponent>() && !go.hasComponent<DebugGraphicsComponent>())
			return;

		if (go.hasComponent<DebugGraphicsComponent>())
			attachDebug(go);
		else
			attachNormal(go);

	}

	void SfSystem::attachDebug(kengine::GameObject & go) {
		const auto & debug = go.getComponent<DebugGraphicsComponent>();
		if (debug.debugType == DebugGraphicsComponent::Text)
			go.attachComponent<SfComponent>(
				debug.text, toWorldPos(debug.startPos), sf::Color(debug.color), debug.textSize, debug.font
				);
		else if (debug.debugType == DebugGraphicsComponent::Line)
			auto & comp = go.attachComponent<SfComponent>(std::make_unique<pse::Shape<sfLine>>(
				toWorldPos(debug.startPos), toWorldPos(debug.endPos), (float)debug.thickness, sf::Color(debug.color)
			));
		else if (debug.debugType == DebugGraphicsComponent::Sphere) {
			auto & comp = go.attachComponent<SfComponent>(std::make_unique<pse::Shape<sf::CircleShape>>(
				(float)debug.radius
				));
			static_cast<pse::Shape<sf::CircleShape> &>(comp.getViewItem()).get().setFillColor(sf::Color(debug.color));
		}
		_engine.addItem(go.getComponent<SfComponent>().getViewItem(), (std::size_t)debug.startPos.y);
	}

	void SfSystem::attachNormal(kengine::GameObject & go) {
		try {
			auto & v = go.hasComponent<SfComponent>() ? go.getComponent<SfComponent>()
				: getResource(go);

			const auto & transform = go.getComponent<kengine::TransformComponent3d>();

			const auto & pos = transform.boundingBox.topLeft;
			v.getViewItem().setPosition(toWorldPos(pos));

			if (!v.isFixedSize()) {
				const auto & size = transform.boundingBox.size;
				v.getViewItem().setSize(toWorldPos(size));
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
		sendTo(packets::KeyStatus::Response{ sf::Keyboard::isKeyPressed(p.key) }, *p.sender);
	}

	void SfSystem::handle(const packets::MouseButtonStatus::Query & p) const noexcept {
		sendTo(packets::MouseButtonStatus::Response{ sf::Mouse::isButtonPressed(p.button) }, *p.sender);
	}

	void SfSystem::handle(const packets::MousePosition::Query & p) noexcept {
		const auto pos = sf::Mouse::getPosition(_engine.getRenderWindow());
		sendTo(packets::MousePosition::Response{ { pos.x, pos.y } }, *p.sender);
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

