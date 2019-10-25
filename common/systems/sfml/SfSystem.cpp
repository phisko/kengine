#include "SfSystem.hpp"

#include <fstream>

#include "EntityManager.hpp"
#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/DebugGraphicsComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/AdjustableComponent.hpp"

#include "packets/Log.hpp"
#include "packets/LuaState.hpp"
#include "lua/plua.hpp"

#include "imgui.h"
#include "imgui-SFML.h"
#include "Shape.hpp"

#include "with.hpp"
#include "to_string.hpp"

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


#include "Export.hpp"
EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new kengine::SfSystem(em);
}

namespace kengine {
	/*
	 * Constructor
	 */
	SfSystem::SfSystem(kengine::EntityManager & em)
		: putils::BaseModule(&em), System(em),
		_screenSize({ 1280, 720 }),
		_tileSize({ 1, 1 }),
		_fullScreen(false),
		_engine((size_t)_screenSize.x, (size_t)_screenSize.y,
			"Kengine",
			_fullScreen ? sf::Style::Fullscreen : sf::Style::Default),
		_em(em) {
		for (auto & e : _em.getEntities())
			handle(kengine::packets::RegisterEntity{ e });

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
				return putils::Point3f{ (float)size.x, (float)size.y };
			};

			lua["getTileSize"] = [this] {
				return putils::Point3f{ _tileSize.x, _tileSize.y };
			};

			lua["getGridSize"] = [this] {
				const auto size = _engine.getRenderWindow().getSize();
				return putils::Point3f{ size.x / _tileSize.x, size.y / _tileSize.y };
			};
		}
		catch (const std::out_of_range &) {}
	}

	/*
	 * System functions
	 */

	void SfSystem::execute() {
		handleEvents();

		ImGui::SFML::Update(_engine.getRenderWindow(), _deltaClock.restart());

		const auto objects = _em.getEntities<ImGuiComponent>();
		for (const auto & [e, comp] : objects)
			comp.display(GImGui);

		updateCameras();
		updateDrawables();

		_engine.update(_shouldClear, [this] { ImGui::SFML::Render(_engine.getRenderWindow()); });
	}

	void SfSystem::updateCameras() noexcept {
		const auto cameras = _em.getEntities<CameraComponent3f, TransformComponent3f>();

		for (const auto & [e, cam, transform] : cameras) {
			_engine.removeView("default");

			const auto name = putils::toString(e.id);
			auto & view = _engine.getView(name);

			auto & frustrum = cam.frustrum; {
				auto pos = toWorldPos(frustrum.position);
				pos.x *= _screenSize.x / _tileSize.x;
				pos.y *= _screenSize.y / _tileSize.y;
				auto size = toWorldSize(frustrum.size);
				size.x *= _screenSize.x / _tileSize.x;
				size.y *= _screenSize.y / _tileSize.y;
				view.setCenter({ pos.x + size.x / 2, pos.y + size.y / 2 });
				view.setSize(size);
			}

			const auto & box = transform.boundingBox;
			view.setViewport(sf::FloatRect{
					(float)box.position.x, (float)box.position.z,
					(float)box.size.x, (float)box.size.z
				});
			_engine.setViewHeight(name, (size_t)box.position.y);
		}
	}

	void SfSystem::updateDrawables() {
		std::vector<Entity::ID> toDetach;

		for (const auto & [e, comp, graphics] : _em.getEntities<SfComponent, GraphicsComponent>())
			if (comp.item != nullptr)
				updateObject(e, *comp.item, graphics, comp.fixedSize);

		for (const auto & [e, comp, graphics] : _em.getEntities<SfComponentDebug, GraphicsComponent>())
			if (comp.item != nullptr)
				updateDebug(e, *comp.item);
	}

	static sf::Color toSfColor(const putils::NormalizedColor & colorNormalized) {
		return sf::Color((int)colorNormalized.r * 255, (int)colorNormalized.g * 255, (int)colorNormalized.b * 255);
	}

	bool SfSystem::updateDebug(EntityView e, pse::ViewItem & item) {
		if (!e.has<DebugGraphicsComponent>())
			return false;

		const auto & debug = e.get<DebugGraphicsComponent>();
		const auto & transform = e.get<TransformComponent3f>();
		switch (debug.debugType) {
			case DebugGraphicsComponent::Text: {
				auto & text = static_cast<pse::Text &>(item);
				text.setString(debug.text.c_str());
				text.setTextSize((unsigned int)debug.offset.size.x);
				text.setPosition(toWorldPos(transform.boundingBox.position + debug.offset.position));
				text.setColor(toSfColor(debug.color));
				break;
			}
			case DebugGraphicsComponent::Line: {
				const auto start = transform.boundingBox.position + debug.offset.position;
				static_cast<pse::Shape<sfLine> &>(item).get().set(
					toWorldPos(start), toWorldPos(start + debug.offset.size), debug.offset.size.y, toSfColor(debug.color)
				);
				break;
			}
			case DebugGraphicsComponent::Sphere: {
				auto & circle = static_cast<pse::Shape<sf::CircleShape> &>(item).get();
				circle.setPosition(toWorldPos(transform.boundingBox.position + debug.offset.position));
				circle.setRadius(transform.boundingBox.size.x * debug.offset.size.x * _tileSize.x);
				circle.setOrigin(circle.getRadius(), circle.getRadius());
				circle.setFillColor(toSfColor(debug.color));
				break;
			}
			case DebugGraphicsComponent::Box: {
				auto & box = static_cast<pse::Shape<sf::RectangleShape> &>(item).get();
				box.setPosition(toWorldPos(transform.boundingBox.position + debug.offset.position));
				box.setSize(toWorldSize(transform.boundingBox.size * debug.offset.size));
				box.setFillColor(toSfColor(debug.color));
				break;
			}
			default:
				throw std::logic_error("[SfSystem] Unknown debug type");
		}

		_engine.setItemHeight(item, (size_t)(transform.boundingBox.position.y + debug.offset.position.y));
		return true;
	}

	void SfSystem::updateObject(EntityView e, pse::ViewItem & item, const GraphicsComponent & graphics, bool fixedSize) {
		const auto & transform = e.get<kengine::TransformComponent3f>();
		updateTransform(item, transform, graphics, fixedSize);

		const auto & appearance = graphics.appearance;
		auto & sprite = static_cast<pse::Sprite &>(item);

		try {
			sprite.setTexture(appearance.str());
		}
		catch (const std::exception & e) {
			std::cerr << putils::termcolor::red << "[SfSystem] Failed to set appearance: " << e.what() << std::endl << putils::termcolor::reset;
		}

		ModelComponent * model = nullptr;
		if (graphics.model != Entity::INVALID_ID)
			model = &_em.getEntity(graphics.model).get<ModelComponent>();

		sprite.setRotation(-transform.yaw - (model != nullptr ? model->yaw : 0.f));
	}

	void SfSystem::updateTransform(pse::ViewItem & item, const TransformComponent3f & transform, const GraphicsComponent & graphics, bool fixedSize) noexcept {
		ModelComponent * model = nullptr;
		if (graphics.model != Entity::INVALID_ID)
			model = &_em.getEntity(graphics.model).get<ModelComponent>();

		const auto center = transform.boundingBox.position + transform.boundingBox.size / 2.f;
		const auto size = transform.boundingBox.size * (model != nullptr ? model->boundingBox.size : putils::Point3f{ 1.f, 1.f, 1.f });
		const putils::Point3f endPos{
			// x' = y*sin(a) + x*cos(a)
			center.x - size.x / 2 + (model != nullptr ? model->boundingBox.position.x : 0.f)
				+ std::cos(transform.yaw) * (model != nullptr ? model->boundingBox.position.x : 0.f)
				+ std::sin(transform.yaw) * (model != nullptr ? model->boundingBox.position.z : 0.f),
			0,
			// y' = y*cos(a) - x*sin(a)
			center.z + size.z / 2 + (model != nullptr ? model->boundingBox.position.z : 0.f)
				+ std::cos(transform.yaw) * (model != nullptr ? model->boundingBox.position.z : 0.f)
				- std::sin(transform.yaw) * (model != nullptr ? model->boundingBox.position.x : 0.f),
		};
		item.setPosition(toWorldPos(endPos));

		const std::size_t height = (unsigned)transform.boundingBox.position.y + (model != nullptr ? (unsigned)model->boundingBox.position.y : 0);
		_engine.setItemHeight(item, height);

		if (!fixedSize)
			item.setSize(toWorldSize(size));

		item.setRotation(-transform.yaw - (model != nullptr ? model->yaw : 0.f));
	}

	void SfSystem::handleEvents() noexcept {
		std::vector<sf::Event> allEvents; {
			sf::Event e;
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
		}

		for (const auto & [e, input] : _em.getEntities<InputComponent>()) {
			for (const auto & e : allEvents)
				switch (e.type) {
				case sf::Event::KeyPressed:
				case sf::Event::KeyReleased:
					if (input.onKey != nullptr && (e.type == sf::Event::KeyPressed || e.type == sf::Event::KeyReleased))
						input.onKey(e.key.code, e.type == sf::Event::KeyPressed);
					break;

				case sf::Event::MouseButtonPressed:
				case sf::Event::MouseButtonReleased:
					if (input.onMouseButton != nullptr) {
						const auto x = e.mouseButton.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
						const auto y = e.mouseButton.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
						input.onMouseButton(e.mouseButton.button, x, y, e.type == sf::Event::MouseButtonPressed);
					}
					break;

				case sf::Event::MouseMoved:
					if (input.onMouseMove != nullptr) {
						const auto x = e.mouseMove.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
						const auto y = e.mouseMove.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
						input.onMouseMove(x, y, 0.f, 0.f);
					}
					break;

				case sf::Event::MouseWheelScrolled:
					if (input.onMouseWheel != nullptr && e.type == sf::Event::MouseWheelScrolled) {
						const auto x = e.mouseWheelScroll.x / _engine.getRenderWindow().getSize().x * _screenSize.x;
						const auto y = e.mouseWheelScroll.y / _engine.getRenderWindow().getSize().y * _screenSize.y;
						input.onMouseWheel(e.mouseWheelScroll.delta, x, y);
					}
					break;

				default:
					break;
				}
		}
	}

	void SfSystem::handle(const packets::RegisterEntity & p) {
		const auto e = p.e;
		if (e.has<SfComponent>() || e.has<GraphicsComponent>())
			attachNormal(e);
		if (e.has<DebugGraphicsComponent>())
			attachDebug(e);
	}

	void SfSystem::attachDebug(Entity e) {
		const auto & debug = e.get<DebugGraphicsComponent>();
		const auto & transform = e.get<TransformComponent3f>();

		auto & comp = e.has<SfComponentDebug>() ?
			e.get<SfComponentDebug>() :
			e.attach<SfComponentDebug>();

		std::unique_ptr<pse::ViewItem> v = nullptr;

		switch (debug.debugType) {
			case DebugGraphicsComponent::Text: {
				v = std::make_unique<pse::Text>(debug.text.c_str(), toWorldPos(transform.boundingBox.position + debug.offset.position), toSfColor(debug.color), (unsigned int)debug.offset.size.x, debug.font.str());
				break;
			}
			case DebugGraphicsComponent::Line: {
				const auto start = transform.boundingBox.position + debug.offset.position;
				v = std::make_unique<pse::Shape<sfLine>>(toWorldPos(start), toWorldPos(start + debug.offset.size), (float)debug.offset.size.y, toSfColor(debug.color));
				break;
			}
			case DebugGraphicsComponent::Sphere: {
				v = std::make_unique<pse::Shape<sf::CircleShape>>(transform.boundingBox.size.x * debug.offset.size.x * _tileSize.x);
				static_cast<pse::Shape<sf::CircleShape> &>(*v).get().setFillColor(toSfColor(debug.color));
				break;
			}
			case DebugGraphicsComponent::Box: {
				v = std::make_unique<pse::Shape<sf::RectangleShape>>();
				break;
			}
			default:
				assert(false && "[SfSystem] Unknown debug type");
		}

		if (v != nullptr) {
			_engine.addItem(*v);
			comp.item = std::move(v);
		}
	}

	void SfSystem::attachNormal(Entity e) {
		pmeta_with(e.get<GraphicsComponent>()) {
			auto & comp = e.attach<SfComponent>();

			const auto & transform = e.get<TransformComponent3f>();
			try {
				auto v = getResource(_.appearance.str());
				v->setPosition(toWorldPos(transform.boundingBox.position));
				v->setSize(toWorldSize(transform.boundingBox.size));
				_engine.addItem(*v, (std::size_t)(transform.boundingBox.position.y));
				comp.item = std::move(v);
			}
			catch (const std::exception &) {
				send(kengine::packets::Log{ putils::concat("[SfSystem] Unknown appearance: ", _.appearance) });
			}
		}
	}

	void SfSystem::handle(const packets::RemoveEntity & p) {
		auto & e = p.e;

		if (e.has<CameraComponent3f>() && _engine.hasView(putils::toString(e.id)))
			_engine.removeView(putils::toString(e.id));

		if (e.has<SfComponent>())
			_engine.removeItem(*e.get<SfComponent>().item);
		if (e.has<SfComponentDebug>())
			_engine.removeItem(*e.get<SfComponentDebug>().item);
	}

	/*
	 * DataPacket handlers
	 */

	void SfSystem::handle(const kengine::packets::RegisterAppearance & p) noexcept {
		_appearances[p.appearance] = p.resource;
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

