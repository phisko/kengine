#include "SFMLSystem.hpp"

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
#include "data/InputBufferComponent.hpp"
#include "data/WindowComponent.hpp"

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

			for (auto [windowEntity, sfWindowComp] : entities.with<SFMLWindowComponent>()) {
				const auto * windowComp = windowEntity.tryGet<WindowComponent>();
				if (windowComp == nullptr) {
					windowEntity.detach<SFMLWindowComponent>();
					continue;
				}

				if (!sfWindowComp.window.isOpen()) {
					if (windowComp->shutdownOnClose)
						stopRunning();
					else
						entities -= windowEntity;
					continue;
				}

				sfWindowComp.window.setSize({ windowComp->size.x, windowComp->size.y });

				processEvents(windowEntity.id, sfWindowComp.window);

				sfWindowComp.window.clear();
				ImGui::SFML::Render(sfWindowComp.window);
				sfWindowComp.window.display();
				ImGui::SFML::Update(sfWindowComp.window, sfDeltaTime);
			}
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

		static void onEntityCreated(Entity & e) noexcept {
			if (const auto * window = e.tryGet<WindowComponent>()) {
				auto & sfWindow = e.attach<SFMLWindowComponent>();
				sfWindow.window.create(
					sf::VideoMode{ window->size.x, window->size.y },
					window->name.c_str(),
					window->fullscreen ? sf::Style::Fullscreen : sf::Style::Default);

				ImGui::SFML::Init(sfWindow.window);
				ImGui::SFML::Update(sfWindow.window, g_deltaClock.restart());
			}
		}

		static void onEntityRemoved(Entity & e) noexcept {
		}

		static void terminate() noexcept {
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
