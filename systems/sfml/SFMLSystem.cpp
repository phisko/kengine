#include "SFMLSystem.hpp"

#include "kengine.hpp"

#include <imgui.h>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include "imgui-sfml/imgui-SFML.h"
#include "SFMLWindowComponent.hpp"
#include "SFMLTextureComponent.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/ImGuiScaleComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"
#include "functions/OnTerminate.hpp"

#include "helpers/logHelper.hpp"

#include "data/CameraComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/ImGuiContextComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"

#include "helpers/cameraHelper.hpp"
#include "helpers/instanceHelper.hpp"

#include "vector.hpp"

#ifndef KENGINE_MAX_VIEWPORTS
# define KENGINE_MAX_VIEWPORTS 8
#endif

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
			kengine_log(Verbose, "Execute", "SFMLSystem");

			const auto sfDeltaTime = g_deltaClock.restart();

			if (g_inputBuffer == nullptr) {
				for (const auto & [e, inputBuffer] : entities.with<InputBufferComponent>()) {
					g_inputBuffer = &inputBuffer;
					break;
				}
			}

			for (auto [window, sfWindow] : entities.with<SFMLWindowComponent>()) {
				kengine_logf(Verbose, "Execute/SFMLSystem", "Processing window %zu", window.id);
				if (!updateWindowState(window, sfWindow))
					continue;
				// We process events after rendering, even though it's not the expected order, because
				// of how the SFML-ImGui binding handles input :(
				render(window, sfWindow);
				processEvents(window.id, sfWindow.window, sfDeltaTime);
			}
		}

		static bool updateWindowState(Entity & windowEntity, SFMLWindowComponent & sfWindowComp) noexcept {
			const auto * windowComp = windowEntity.tryGet<WindowComponent>();
			if (windowComp == nullptr) {
				kengine_logf(Verbose, "Execute/SFMLSystem", "%zu: destroying window as WindowComponent was removed", windowEntity.id);
				windowEntity.detach<SFMLWindowComponent>();
				return false;
			}

			if (!sfWindowComp.window.isOpen()) {
				kengine_logf(Verbose, "Execute/SFMLSystem", "%zu: window was closed", windowEntity.id);
				if (windowComp->shutdownOnClose)
					stopRunning();
				else
					entities -= windowEntity;
				return false;
			}
			
			sfWindowComp.window.setSize({ windowComp->size.x, windowComp->size.y });
			return true;
		}

		static void processEvents(EntityID window, sf::RenderWindow & sfWindow, sf::Time deltaTime) noexcept {
			sf::Event event;
			while (sfWindow.pollEvent(event)) {
				ImGui::SFML::ProcessEvent(sfWindow, event);

				if (event.type == sf::Event::Closed)
					sfWindow.close();

				processInput(window, event);
			}
			ImGui::SFML::Update(sfWindow, deltaTime);
		}

		static void processInput(EntityID window, const sf::Event & e) noexcept {
			if (g_inputBuffer == nullptr)
				return;
			
			switch (e.type) {
				case sf::Event::KeyPressed:
				case sf::Event::KeyReleased: {
					if (ImGui::GetIO().WantCaptureKeyboard)
						break;

					g_inputBuffer->keys.push_back(InputBufferComponent::KeyEvent{
						.window = window,
						.key = e.key.code,
						.pressed = e.type == sf::Event::KeyPressed
					});
					break;
				}
				case sf::Event::MouseMoved: {
					if (ImGui::GetIO().WantCaptureMouse)
						break;

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
				}
				case sf::Event::MouseButtonPressed:
				case sf::Event::MouseButtonReleased: {
					if (ImGui::GetIO().WantCaptureMouse)
						break;

					g_inputBuffer->clicks.push_back(InputBufferComponent::ClickEvent{
						.window = window,
						.pos = { (float)e.mouseButton.x, (float)e.mouseButton.y },
						.button = e.mouseButton.button,
						.pressed = e.type == sf::Event::MouseButtonPressed
					});
					break;
				}
				case sf::Event::MouseWheelScrolled: {
					if (ImGui::GetIO().WantCaptureMouse)
						return;

					g_inputBuffer->scrolls.push_back(InputBufferComponent::MouseScrollEvent{
						.window = window,
						.xoffset = 0,
						.yoffset = e.mouseWheelScroll.delta,
						.pos = { (float)e.mouseWheelScroll.x, (float)e.mouseWheelScroll.y }
					});
					break;
				}
				default:
					// don't assert, this could be a joystick event or something
					break;
			}
		}

		static void render(const Entity & windowEntity, SFMLWindowComponent & sfWindow) noexcept {
			sfWindow.window.clear();

			struct ToBlit {
				const sf::RenderTexture * renderTexture;
				const ViewportComponent * viewport;
			};
			putils::vector<ToBlit, KENGINE_MAX_VIEWPORTS> toBlit;

			for (auto [e, cam, viewport] : entities.with<CameraComponent, ViewportComponent>()) {
				if (viewport.window == INVALID_ID) {
					kengine_logf(Log, "OpenGLSystem", "Setting target window for ViewportComponent in %zu", e.id);
					viewport.window = windowEntity.id;
				}
				else if (viewport.window != windowEntity.id)
					continue;

				sf::RenderTexture * renderTexture = (sf::RenderTexture *)viewport.renderTexture;
				if (viewport.renderTexture == ViewportComponent::INVALID_RENDER_TEXTURE) {
					renderTexture = new sf::RenderTexture;
					renderTexture->create(viewport.resolution.x, viewport.resolution.y);
					viewport.renderTexture = renderTexture;
				}

				renderTexture->setView(sf::View{
					{ cam.frustum.position.x, cam.frustum.position.y },
					{ cam.frustum.size.x, cam.frustum.size.y }
				});
				renderToTexture(*renderTexture);
				toBlit.push_back(ToBlit{ renderTexture, &viewport });
			}

			std::sort(toBlit.begin(), toBlit.end(), [](const ToBlit & lhs, const ToBlit & rhs) noexcept {
				return lhs.viewport->zOrder < rhs.viewport->zOrder;
			});

			for (const auto & blit : toBlit) {
				auto renderTextureSprite = createRenderTextureSprite(*blit.renderTexture, sfWindow.window, *blit.viewport);
				sfWindow.window.draw(std::move(renderTextureSprite));
			}
		
			ImGui::SFML::Render(sfWindow.window);
			sfWindow.window.display();
		}

		static void renderToTexture(sf::RenderTexture & renderTexture) noexcept {
			renderTexture.clear();

			struct ZOrderedSprite {
				sf::Sprite sprite;
				float height;
			};
			std::vector<ZOrderedSprite> sprites;

			for (const auto & [e, transform, graphics] : entities.with<TransformComponent, GraphicsComponent>()) {
				auto sprite = createEntitySprite(e, transform, graphics);
				if (sprite != std::nullopt)
					sprites.push_back({ .sprite = std::move(*sprite), .height = transform.boundingBox.position.y });
			}

			std::sort(sprites.begin(), sprites.end(), [](const auto & lhs, const auto & rhs) noexcept { return lhs.height > rhs.height; });
			for (const auto & sprite : sprites)
				renderTexture.draw(sprite.sprite);

			renderTexture.display();
		}

		static std::optional<sf::Sprite> createEntitySprite(const Entity & e, const TransformComponent & transform, const GraphicsComponent & graphics) noexcept {
			const auto * texture = instanceHelper::tryGetModel<SFMLTextureComponent>(e);
			if (texture == nullptr)
				return std::nullopt;
			
			sf::Sprite sprite(texture->texture);
			sprite.setColor(sf::Color(toColor(graphics.color).rgba));
			sprite.setPosition(transform.boundingBox.position.x - transform.boundingBox.size.x / 2.f, transform.boundingBox.position.y - transform.boundingBox.size.y / 2.f);

			const auto textureSize = texture->texture.getSize();
			sprite.setScale(transform.boundingBox.size.x / textureSize.x, transform.boundingBox.size.y / textureSize.y);
			sprite.setRotation(transform.yaw);
			return sprite;
		}

		static sf::Sprite createRenderTextureSprite(const sf::RenderTexture & renderTexture, const sf::RenderWindow & window, const ViewportComponent & viewport) noexcept {
			sf::Sprite renderTextureSprite(renderTexture.getTexture());

			const auto screenSize = window.getSize();
			const auto box = cameraHelper::convertToScreenPercentage({ viewport.boundingBox.position, viewport.boundingBox.size }, { (float)screenSize.x, (float)screenSize.y }, viewport);
			renderTextureSprite.setPosition(screenSize.x * box.position.x, screenSize.y * box.position.y);

			const auto textureSize = renderTexture.getTexture().getSize();
			renderTextureSprite.setScale(screenSize.x / textureSize.x * box.size.x, screenSize.y / textureSize.y * box.size.y);

			return renderTextureSprite;
		}

		static void onEntityCreated(Entity & e) noexcept {
			if (const auto * window = e.tryGet<WindowComponent>())
				createWindow(e, *window);

			if (const auto * model = e.tryGet<ModelComponent>())
				createTexture(e, *model);
		}

		static void createWindow(Entity & e, const WindowComponent & windowComp) noexcept {
			kengine_logf(Log, "SFML", "Creating window '%s'", windowComp.name.c_str());
			
			auto & sfWindow = e.attach<SFMLWindowComponent>();
			sfWindow.window.create(
				sf::VideoMode{ windowComp.size.x, windowComp.size.y },
				windowComp.name.c_str(),
				windowComp.fullscreen ? sf::Style::Fullscreen : sf::Style::Default);

			ImGui::SFML::Init(sfWindow.window);

			e += ImGuiContextComponent{ ImGui::GetCurrentContext() };
#if 0
			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;
#endif
			ImGui::SFML::Update(sfWindow.window, g_deltaClock.restart());
		}

		static void createTexture(Entity & e, const ModelComponent & model) noexcept {
			sf::Texture texture;
			if (texture.loadFromFile(model.file.c_str())) {
				kengine_logf(Log, "SFML", "Loaded texture for '%s'", model.file.c_str());
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
