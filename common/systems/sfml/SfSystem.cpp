#include "SfSystem.hpp"

#include "EntityManager.hpp"
#include "components/TransformComponent.hpp"
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

#include <TGUI/TGUI.hpp>
#include "TGUI/Widgets/Button.hpp"
#include <TGUI/Loading/Theme.hpp>

#include "with.hpp"

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

	static const GraphicsComponent::Layer * getLayer(const std::string & name, const kengine::GraphicsComponent & graphics) {
		const auto it = std::find_if(graphics.layers.begin(), graphics.layers.end(), [&name](auto && layer) { return layer.name == name; });
		if (it == graphics.layers.end())
			return nullptr;
		return &*it;
	}

	void SfSystem::updateDrawables() {
		for (const auto go : _em.getGameObjects<kengine::GUIComponent>())
			updateGUIElement(*go);

		std::vector<kengine::GameObject *> toDetach;

		for (const auto go : _em.getGameObjects<SfComponent>()) {
			std::vector<const SfComponent::Layer *> toRemove;

			const GraphicsComponent * graphics = nullptr;
			auto & comp = go->getComponent<SfComponent>();
			auto & items = comp.viewItems;
			for (const auto & layer : items) {
				if (layer.name == "__debug__") {
					if (!updateDebug(*go, *layer.item))
						toRemove.push_back(&layer);
					continue;
				} 

				if (graphics == nullptr)
					graphics = &go->getComponent<GraphicsComponent>();
				const auto graphicsLayer = getLayer(layer.name, *graphics);
				if (graphicsLayer == nullptr)
					toRemove.push_back(&layer);
				else
					updateObject(*go, *layer.item, *graphicsLayer, layer.fixedSize);
			}

			for (const auto layer : toRemove) {
				const auto it = std::find_if(items.begin(), items.end(), [layer](auto && it) { return &it == layer; });
				_engine.removeItem(*it->item);
				items.erase(it);
			}

			if (go->hasComponent<GraphicsComponent>()) {
				auto & transform = go->getComponent<TransformComponent3d>();
				for (const auto & layer : go->getComponent<GraphicsComponent>().layers) {
					const auto it = std::find_if(items.begin(), items.end(), [&layer](auto && item) { return layer.name == item.name; });
					if (it == items.end())
						attachLayer(comp, layer, transform.boundingBox);
				}
			}

			if (items.empty())
				toDetach.push_back(go);
		}

		for (const auto go : toDetach) {
			for (const auto & layer : go->getComponent<SfComponent>().viewItems)
				_engine.removeItem(*layer.item);
			go->detachComponent<SfComponent>();
		}
	}

	bool SfSystem::updateDebug(kengine::GameObject & go, pse::ViewItem & item) {
		if (!go.hasComponent<DebugGraphicsComponent>())
			return false;

		const auto & debug = go.getComponent<kengine::DebugGraphicsComponent>();
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

		return true;
	}

	static putils::Point3d getLayerSize(const putils::Point3d & transformSize, const putils::Point3d & layerSize) {
		return {
			transformSize.x * layerSize.x,
			transformSize.y * layerSize.y,
			transformSize.z * layerSize.z
		};
	}

	void SfSystem::updateObject(kengine::GameObject & go, pse::ViewItem & item, const GraphicsComponent::Layer & layer, bool fixedSize) {
		const auto & transform = go.getComponent<kengine::TransformComponent3d>();
		updateTransform(go, item, transform, layer, fixedSize);

		const auto & graphics = go.getComponent<kengine::GraphicsComponent>();
		const auto & appearance = layer.appearance;
		auto & sprite = static_cast<pse::Sprite &>(item);

		try {
			sprite.setTexture(appearance);
		}
		catch (const std::exception & e) {
			std::cerr << "[SfSystem] Failed to set appearance: " << e.what() << std::endl;
		}

		sprite.setRotation(-transform.yaw - layer.yaw);

		if (graphics.repeated) {
			const auto & box = transform.boundingBox;

			const sf::FloatRect rect(toWorldPos(box.topLeft), toWorldPos(getLayerSize(box.size, layer.boundingBox.size)));
			const sf::IntRect r(rect);

			sprite.repeat(r);
		}
		else
			sprite.unrepeat();
	}

	void SfSystem::updateGUIElement(kengine::GameObject & go) noexcept {
		const auto & gui = go.getComponent<kengine::GUIComponent>();
		if (gui.guiType == GUIComponent::Text || gui.guiType == GUIComponent::Button) {
			auto & element = _guiElements[&go];
			auto & win = _engine.getGui();
			element.frame->setPosition(tgui::bindWidth(win) * gui.boundingBox.topLeft.x, tgui::bindHeight(win) * gui.boundingBox.topLeft.z);
			element.frame->setSize(tgui::bindWidth(win) * gui.boundingBox.size.x, tgui::bindHeight(win) * gui.boundingBox.size.z);
			element.label->setText(gui.text);
		}
	}

	void SfSystem::updateTransform(kengine::GameObject & go, pse::ViewItem & item, const TransformComponent3d & transform, const GraphicsComponent::Layer & layer, bool fixedSize) noexcept {
		const auto center = transform.boundingBox.getCenter();
		const auto size = getLayerSize(transform.boundingBox.size, layer.boundingBox.size);
		const putils::Point3d endPos{
			center.x - size.x / 2 + std::cos(transform.yaw) * layer.boundingBox.topLeft.x,
			0,
			center.z - size.x / 2 + std::sin(transform.yaw) * layer.boundingBox.topLeft.z,
		};
		item.setPosition(toWorldPos(endPos));

		const std::size_t height = transform.boundingBox.topLeft.y + layer.boundingBox.topLeft.y;
		_engine.setItemHeight(item, height);

		if (!fixedSize)
			item.setSize(toWorldPos(size));

		item.setRotation(-transform.yaw - layer.yaw);
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
				if (input.onMouseButton != nullptr && (e.type == sf::Event::MouseButtonPressed || e.type == sf::Event::MouseButtonReleased)) {
					const auto x = (double)e.mouseButton.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
					const auto y = (double)e.mouseButton.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
					input.onMouseButton(e.mouseButton.button, x, y, e.type == sf::Event::MouseButtonPressed);
				}
				else if (input.onMouseMove != nullptr && e.type == sf::Event::MouseMoved) {
					const auto x = (double)e.mouseMove.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
					const auto y = (double)e.mouseMove.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
					input.onMouseMove(x, y);
				}
				else if (input.onKey != nullptr && (e.type == sf::Event::KeyPressed || e.type == sf::Event::KeyReleased))
					input.onKey(e.key.code, e.type == sf::Event::KeyPressed);
				else if (input.onMouseWheel != nullptr && e.type == sf::Event::MouseWheelScrolled)
					input.onMouseWheel(e.mouseWheelScroll.delta, e.mouseWheelScroll.x, e.mouseWheelScroll.y);
			}
		}
	}

	void SfSystem::handle(const kengine::packets::RegisterGameObject & p) {
		auto & go = p.go;

		if (go.hasComponent<SfComponent>() || go.hasComponent<GraphicsComponent>())
			attachNormal(go);
		if (go.hasComponent<DebugGraphicsComponent>())
			attachDebug(go);
		if (go.hasComponent<GUIComponent>())
			attachGUI(go);
	}

	void SfSystem::attachDebug(kengine::GameObject & go) {
		const auto & debug = go.getComponent<DebugGraphicsComponent>();

		auto & comp = go.hasComponent<SfComponent>() ?
			go.getComponent<SfComponent>() :
			go.attachComponent<SfComponent>();

		std::unique_ptr<pse::ViewItem> v = nullptr;

		if (debug.debugType == DebugGraphicsComponent::Text)
			v = std::make_unique<pse::Text>(debug.text, toWorldPos(debug.startPos), sf::Color(debug.color), debug.textSize, debug.font);
		else if (debug.debugType == DebugGraphicsComponent::Line)
			v = std::make_unique<pse::Shape<sfLine>>(toWorldPos(debug.startPos), toWorldPos(debug.endPos), (float)debug.thickness, sf::Color(debug.color));
		else if (debug.debugType == DebugGraphicsComponent::Sphere) {
			v = std::make_unique<pse::Shape<sf::CircleShape>>((float)debug.radius);
			static_cast<pse::Shape<sf::CircleShape> &>(*v).get().setFillColor(sf::Color(debug.color));
		}

		if (v != nullptr) {
			_engine.addItem(*v, (std::size_t)debug.startPos.y);
			comp.viewItems.push_back({ "__debug__", std::move(v) });
		}
	}

	void SfSystem::attachGUI(kengine::GameObject & go) {
		static auto theme = _config.find("theme") != _config.end() ? tgui::Theme::create(_config["theme"]) : nullptr;
		const auto & gui = go.getComponent<GUIComponent>();

		auto & win = _engine.getGui();

		auto & element = _guiElements[&go];
		if (gui.guiType == GUIComponent::Button || gui.guiType == GUIComponent::Text) {
			std::shared_ptr<tgui::Button> button = theme != nullptr ? theme->load("Button") : tgui::Button::create();
			button->setPosition(tgui::bindWidth(win) * gui.boundingBox.topLeft.x, tgui::bindHeight(win) * gui.boundingBox.topLeft.z);
			button->setSize(tgui::bindWidth(win) * gui.boundingBox.size.x, tgui::bindHeight(win) * gui.boundingBox.size.z);

			std::shared_ptr<tgui::Label> label = theme != nullptr ? theme->load("Label") : tgui::Label::create();
			label->setText(gui.text);
			label->setPosition(tgui::bindPosition(button));
			label->setSize(tgui::bindSize(button));
			label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
			label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
			if (gui.onClick != nullptr)
				label->connect("clicked", gui.onClick);

			element.frame = std::move(button);
			element.label = std::move(label);
		}

		if (element.frame != nullptr)
			_engine.addItem(element.frame);
		if (element.label != nullptr)
			_engine.addItem(element.label);
	}

	void SfSystem::attachNormal(kengine::GameObject & go) {
		pmeta_with(go.getComponent<GraphicsComponent>()) {
			auto & comp = go.attachComponent<SfComponent>();

			const auto & transform = go.getComponent<kengine::TransformComponent3d>();
			for (const auto & layer : _.layers)
				attachLayer(comp, layer, transform.boundingBox);
		}
	}

	void SfSystem::attachLayer(SfComponent & comp, const GraphicsComponent::Layer & layer, const putils::Rect3d & boundingBox) {
		try {
			auto v = getResource(layer.appearance);
			v->setPosition(toWorldPos(boundingBox.topLeft + layer.boundingBox.topLeft));
			v->setSize(toWorldPos(getLayerSize(boundingBox.size, layer.boundingBox.size)));
			_engine.addItem(*v, (std::size_t) boundingBox.topLeft.y + layer.boundingBox.topLeft.y);
			comp.viewItems.push_back({ layer.name, std::move(v) });
		} catch (const std::exception & e) {
			send(kengine::packets::Log{ putils::concat("[SfSystem] Unknown appearance: ", layer.appearance) });
		}
	}

	void SfSystem::handle(const kengine::packets::RemoveGameObject & p) {
		auto & go = p.go;

		if (go.hasComponent<kengine::CameraComponent3d>() && _engine.hasView(go.getName()))
			_engine.removeView(go.getName());

		if (go.hasComponent<GUIComponent>()) {
			const auto & element = _guiElements[&go];
			_engine.getGui().remove(element.frame);
			_engine.getGui().remove(element.label);
			_guiElements.erase(&go);
		}

		if (!go.hasComponent<SfComponent>())
			return;

		const auto & comp = go.getComponent<SfComponent>();
		for (const auto & layer : comp.viewItems)
			_engine.removeItem(*layer.item);
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

	std::unique_ptr<pse::Sprite> SfSystem::getResource(const std::string & appearance) {
		const auto & str = _appearances.find(appearance) != _appearances.end()
			? _appearances.at(appearance)
			: appearance;

		return std::make_unique<pse::Sprite>(str, sf::Vector2f{ 0, 0 }, sf::Vector2f{ 16, 16 });
	}
}

