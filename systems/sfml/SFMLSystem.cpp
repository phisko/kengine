#include "SFMLSystem.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include "kengine.hpp"

#include "imgui-sfml/imgui-SFML.h"

#include "data/AdjustableComponent.hpp"
#include "data/ImGuiScaleComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"
#include "functions/OnTerminate.hpp"

#include "helpers/logHelper.hpp"

#include "SFMLWindowComponent.hpp"
#include "SFMLTextureComponent.hpp"
#include "data/GraphicsComponent.hpp"

#include "data/InputBufferComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/WindowComponent.hpp"
#include "helpers/instanceHelper.hpp"
#include "helpers/resourceHelper.hpp"

namespace kengine {
	struct sfml {
		static void init(Entity & e) noexcept {
			kengine_log(Log, "Init", "SFMLSystem");

			e += functions::Execute{ execute };
			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::OnTerminate{ terminate };

			auto & scale = e.attach<ImGuiScaleComponent>();

			e += AdjustableComponent{
				"ImGui", {
					{ "scale", &scale.scale }
				}
			};
		}

		static void execute(float deltaTime) noexcept {
			const auto sfDeltaTime = g_deltaClock.restart();

			if (g_inputBuffer == nullptr) {
				for (const auto & [e, inputBuffer] : entities.with<InputBufferComponent>()) {
					g_inputBuffer = &inputBuffer;
					break;
				}
			}

			for (auto [window, sfWindow] : entities.with<SFMLWindowComponent>()) {
				updateWindowState(window, sfWindow);
				processEvents(window.id, sfWindow.window);
				render(sfWindow, sfDeltaTime);
			}
		}

		static bool updateWindowState(Entity & windowEntity, SFMLWindowComponent & sfWindowComp) noexcept {
			const auto * windowComp = windowEntity.tryGet<WindowComponent>();
			if (windowComp == nullptr) {
				windowEntity.detach<SFMLWindowComponent>();
				return false;
			}

			if (!sfWindowComp.window.isOpen()) {
				if (windowComp->shutdownOnClose)
					stopRunning();
				else
					entities -= windowEntity;
				return false;
			}
			
			sfWindowComp.window.setSize({ windowComp->size.x, windowComp->size.y });
			return true;
		}

		static void processEvents(EntityID window, sf::RenderWindow & sfWindow) noexcept {
			sf::Event event;
			while (sfWindow.pollEvent(event)) {
				ImGui::SFML::ProcessEvent(sfWindow, event);

				if (event.type == sf::Event::Closed)
					sfWindow.close();

				processInput(window, event);
			}
		}

		static void processInput(EntityID window, const sf::Event & e) {
			if (g_inputBuffer == nullptr)
				return;
			
			switch (e.type) {
				case sf::Event::KeyPressed:
				case sf::Event::KeyReleased:
					g_inputBuffer->keys.push_back(InputBufferComponent::KeyEvent{
						.window = window,
						.key = e.key.code,
						.pressed = e.type == sf::Event::KeyPressed
					});
					break;
				case sf::Event::MouseMoved:
					static putils::Point2f previousPos{ 0.f, 0.f };

					const putils::Point2f pos{ (float)e.mouseMove.x, (float)e.mouseMove.y };
					const putils::Point2f rel = pos - previousPos;
					previousPos = pos;

					g_inputBuffer->moves.push_back(InputBufferComponent::MouseMoveEvent{
						.window = window,
						.pos = pos,
						.rel = rel
					});
					break;
				case sf::Event::MouseButtonPressed:
				case sf::Event::MouseButtonReleased:
					g_inputBuffer->clicks.push_back(InputBufferComponent::ClickEvent{
						.window = window,
						.pos = { (float)e.mouseButton.x, (float)e.mouseButton.y },
						.button = e.mouseButton.button,
						.pressed = e.type == sf::Event::MouseButtonPressed
					});
					break;
				case sf::Event::MouseWheelScrolled:
					g_inputBuffer->scrolls.push_back(InputBufferComponent::MouseScrollEvent{
						.window = window,
						.xoffset = 0,
						.yoffset = e.mouseWheelScroll.delta,
						.pos = { (float)e.mouseWheelScroll.x, (float)e.mouseWheelScroll.y }
					});
					break;
				default:
					// don't assert, this could be a joystick event or something
					break;
			}
		}

		static void render(SFMLWindowComponent & window, sf::Time deltaTime) {
			window.window.clear();
			ImGui::SFML::Render(window.window);

			struct ZOrderedSprite {
				sf::Sprite sprite;
				float height;
			};
			std::vector<ZOrderedSprite> sprites;

			for (const auto & [e, transform, graphics] : entities.with<TransformComponent, GraphicsComponent>()) {
				const auto * texture = instanceHelper::tryGetModel<SFMLTextureComponent>(e);
				if (texture == nullptr)
					continue;

				sf::Sprite sprite(texture->texture);
				sprite.setColor(sf::Color(toColor(graphics.color).rgba));
				sprite.setPosition(transform.boundingBox.position.x, transform.boundingBox.position.z);
				sprite.setScale(transform.boundingBox.size.x, transform.boundingBox.size.y);
				sprite.setRotation(transform.yaw);
				sprites.push_back({ .sprite = std::move(sprite), .height = transform.boundingBox.position.y });
			}

			std::sort(sprites.begin(), sprites.end(), [](const auto & lhs, const auto & rhs) { return lhs.height > rhs.height; });
			for (const auto & sprite : sprites)
				window.window.draw(sprite.sprite);
			
			window.window.display();
			ImGui::SFML::Update(window.window, deltaTime);
		}

		static void onEntityCreated(Entity & e) noexcept {
			if (const auto * window = e.tryGet<WindowComponent>())
				createWindow(e, *window);

			if (const auto * model = e.tryGet<ModelComponent>())
				createTexture(e, *model);
		}

		static void createWindow(Entity & e, const WindowComponent & windowComp) noexcept {
			kengine_log(Log, "SFML", "Creating window '%s'", windowComp.name.c_str());
			
			auto & sfWindow = e.attach<SFMLWindowComponent>();
			sfWindow.window.create(
				sf::VideoMode{ windowComp.size.x, windowComp.size.y },
				windowComp.name.c_str(),
				windowComp.fullscreen ? sf::Style::Fullscreen : sf::Style::Default);

			ImGui::SFML::Init(sfWindow.window);
			ImGui::SFML::Update(sfWindow.window, g_deltaClock.restart());
		}

		static void createTexture(Entity & e, const ModelComponent & model) noexcept {
			sf::Texture texture;
			if (texture.loadFromFile(model.file.c_str())) {
				kengine_log(Log, "SFML", "Loaded texture for '%s'", model.file.c_str());
				e += SFMLTextureComponent{ std::move(texture) };
			}
		}

		static void onEntityRemoved(Entity & e) noexcept {
			if (const auto * sfWindow = e.tryGet<SFMLWindowComponent>()) {
				kengine_log(Log, "SFML", "Shutting down ImGui for window");
				ImGui::SFML::Shutdown(sfWindow->window);
			}
		}

		static void terminate() noexcept {
			kengine_log(Log, "Terminate/SFML", "Shutting down ImGui");
			ImGui::SFML::Shutdown();
		}

		static inline sf::Clock g_deltaClock;
		static inline InputBufferComponent * g_inputBuffer = nullptr;
	};
}

namespace kengine {
	EntityCreator * SFMLSystem() noexcept {
		return [](Entity & e) noexcept {
			sfml::init(e);
		};
	}
}
