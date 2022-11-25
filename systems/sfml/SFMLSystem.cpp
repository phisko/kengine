#include "SFMLSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// sfml
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>

// putils
#include "forward_to.hpp"
#include "vector.hpp"

// kengine data
#include "data/CameraComponent.hpp"
#include "data/DebugGraphicsComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/ImGuiContextComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/ImGuiScaleComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/cameraHelper.hpp"
#include "helpers/instanceHelper.hpp"
#include "helpers/isRunning.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

// impl
#include "imgui-sfml/imgui-SFML.h"
#include "SFMLWindowComponent.hpp"
#include "SFMLTextureComponent.hpp"

#ifndef KENGINE_MAX_VIEWPORTS
# define KENGINE_MAX_VIEWPORTS 8
#endif

namespace {
	using namespace kengine;

	template<typename PointType>
	struct VectorConverter {
		const PointType & p;

		template<typename T>
		operator sf::Vector2<T>() const noexcept {
			return { p.x, p.y };
		}

		template<typename T>
		operator sf::Vector3<T>() const noexcept {
			return { p.x, p.y, p.z };
		}
	};
#define convertVector(vec) VectorConverter<decltype(vec)>(vec)

	sf::Color convertColor(const putils::NormalizedColor & color) noexcept {
		const auto tmp = toColor(color);
		return { tmp.r, tmp.g, tmp.b, tmp.a };
	}

	struct SFMLSystem {
		entt::registry & r;
		putils::vector<entt::scoped_connection, 2> connections;

		sf::Clock deltaClock;
		InputBufferComponent * inputBuffer = nullptr;

		SFMLSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "SFMLSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			auto & scale = e.emplace<ImGuiScaleComponent>();

			e.emplace<AdjustableComponent>() = {
				"ImGui", {
					{ "scale", &scale.scale }
				}
			};

			connections.emplace_back(r.on_construct<WindowComponent>().connect<&SFMLSystem::createWindow>(this));
			connections.emplace_back(r.on_construct<ModelComponent>().connect<createTexture>());
		}

		~SFMLSystem() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Terminate/SFML", "Shutting down ImGui");
			ImGui::SFML::Shutdown();
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "SFMLSystem");

			const auto sfDeltaTime = deltaClock.restart();

			if (inputBuffer == nullptr) {
				for (const auto & [e, buffer] : r.view<InputBufferComponent>().each()) {
					inputBuffer = &buffer;
					break;
				}
			}

			for (auto [window, sfWindow] : r.view<SFMLWindowComponent>().each()) {
				kengine_logf(r, Verbose, "Execute/SFMLSystem", "Processing window %zu", window);
				if (!updateWindowState(window, sfWindow))
					continue;
				// We process events after rendering, even though it's not the expected order, because
				// of how the SFML-ImGui binding handles input :(
				render(window, sfWindow);
				processEvents(window, *sfWindow.window, sfDeltaTime);
			}
		}

		bool updateWindowState(entt::entity windowEntity, SFMLWindowComponent & sfWindowComp) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto * windowComp = r.try_get<WindowComponent>(windowEntity);
			if (windowComp == nullptr) {
				kengine_logf(r, Verbose, "Execute/SFMLSystem", "%zu: destroying window as WindowComponent was removed", windowEntity);
				r.remove<SFMLWindowComponent>(windowEntity);
				return false;
			}

			if (!sfWindowComp.window->isOpen()) {
				kengine_logf(r, Verbose, "Execute/SFMLSystem", "%zu: window was closed", windowEntity);
				r.destroy(windowEntity);
				return false;
			}

			sfWindowComp.window->setSize(convertVector(windowComp->size));
			return true;
		}

		void processEvents(entt::entity window, sf::RenderWindow & sfWindow, sf::Time deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			sf::Event event;
			while (sfWindow.pollEvent(event)) {
				ImGui::SFML::ProcessEvent(sfWindow, event);

				if (event.type == sf::Event::Closed)
					sfWindow.close();

				processInput(window, event);
			}
			ImGui::SFML::Update(sfWindow, deltaTime);
		}

		putils::Point2f previousMousePos{ 0.f, 0.f };
		void processInput(entt::entity window, const sf::Event & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (inputBuffer == nullptr)
				return;

			switch (e.type) {
				case sf::Event::KeyPressed:
				case sf::Event::KeyReleased: {
					if (ImGui::GetIO().WantCaptureKeyboard)
						break;

					inputBuffer->keys.push_back(InputBufferComponent::KeyEvent{
						.window = window,
						.key = e.key.code,
						.pressed = e.type == sf::Event::KeyPressed
					});
					break;
				}
				case sf::Event::MouseMoved: {
					if (ImGui::GetIO().WantCaptureMouse)
						break;

					const putils::Point2f pos{ (float)e.mouseMove.x, (float)e.mouseMove.y };
					const putils::Point2f rel = pos - previousMousePos;
					previousMousePos = pos;

					inputBuffer->moves.push_back(InputBufferComponent::MouseMoveEvent{
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

					inputBuffer->clicks.push_back(InputBufferComponent::ClickEvent{
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

					inputBuffer->scrolls.push_back(InputBufferComponent::MouseScrollEvent{
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

		void render(entt::entity windowEntity, SFMLWindowComponent & sfWindow) noexcept {
			KENGINE_PROFILING_SCOPE;

			sfWindow.window->clear();

			struct ToBlit {
				const sf::RenderTexture * renderTexture;
				const ViewportComponent * viewport;
			};
			putils::vector<ToBlit, KENGINE_MAX_VIEWPORTS> toBlit;

			for (auto [e, cam, viewport] : r.view<CameraComponent, ViewportComponent>().each()) {
				if (viewport.window == entt::null) {
					kengine_logf(r, Log, "SFMLSystem", "Setting target window for ViewportComponent in %zu", e);
					viewport.window = windowEntity;
				}
				else if (viewport.window != windowEntity)
					continue;

				sf::RenderTexture * renderTexture = (sf::RenderTexture *)viewport.renderTexture;
				if (viewport.renderTexture == ViewportComponent::INVALID_RENDER_TEXTURE) {
					renderTexture = new sf::RenderTexture;
					renderTexture->create(viewport.resolution.x, viewport.resolution.y);
					viewport.renderTexture = (ViewportComponent::RenderTexture)renderTexture;
				}

				renderTexture->setView(sf::View{ convertVector(cam.frustum.position), convertVector(cam.frustum.size) });
				renderToTexture(*renderTexture);
				toBlit.push_back(ToBlit{ renderTexture, &viewport });
			}

			std::ranges::sort(toBlit, [](const ToBlit & lhs, const ToBlit & rhs) noexcept {
				return lhs.viewport->zOrder < rhs.viewport->zOrder;
			});

			for (const auto & blit : toBlit) {
				auto renderTextureSprite = createRenderTextureSprite(*blit.renderTexture, *sfWindow.window, *blit.viewport);
				sfWindow.window->draw(std::move(renderTextureSprite));
			}

			ImGui::SFML::Render(*sfWindow.window);
			sfWindow.window->display();
		}

		void renderToTexture(sf::RenderTexture & renderTexture) noexcept {
			KENGINE_PROFILING_SCOPE;

			renderTexture.clear();

			struct Drawables {
				std::vector<sf::Sprite> sprites;
				std::vector<sf::CircleShape> circles;
				std::vector<sf::RectangleShape> rectangles;
				std::vector<sf::Text> texts;

				struct Line {
					sf::Vertex vertices[2];
				};
				std::vector<Line> lines;

				struct Element {
					enum {
						Sprite,
						Circle,
						Rectangle,
						Text,
						Line
					} type;
					size_t index;
					float height;
				};
				std::vector<Element> orderedElements;
			} drawables;

			for (const auto & [e, transform, graphics] : r.view<TransformComponent, GraphicsComponent>().each()) {
				auto sprite = createEntitySprite(e, transform, graphics);
				if (sprite != std::nullopt) {
					drawables.sprites.emplace_back(std::move(*sprite));
					drawables.orderedElements.push_back({
						.type = Drawables::Element::Sprite,
						.index = drawables.sprites.size() - 1,
						.height = transform.boundingBox.position.y });
				}
			}

			for (const auto & [e, transform, debug] : r.view<TransformComponent, DebugGraphicsComponent>().each()) {
				for (const auto & element : debug.elements) {
					const sf::Color color(convertColor(element.color));

					auto pos = element.pos;
					if (element.referenceSpace == DebugGraphicsComponent::ReferenceSpace::Object)
						pos += transform.boundingBox.position;

					const auto height = pos.z;

					using ElementType = DebugGraphicsComponent::Type;
					switch (element.type) {
						case ElementType::Line: {
							Drawables::Line line;
							line.vertices[0].color = color;
							line.vertices[0].position = convertVector(element.pos); // lines are always in world space
							line.vertices[1].color = color;
							line.vertices[1].position = convertVector(element.line.end);
							drawables.lines.push_back(std::move(line));
							drawables.orderedElements.push_back({
								.type = Drawables::Element::Line,
								.index = drawables.lines.size() - 1,
								.height = height });
							break;
						}
						case ElementType::Sphere: {
							sf::CircleShape circle(element.sphere.radius);
							circle.setFillColor(color);
							const sf::Vector2f sfPos = convertVector(pos);
							circle.setPosition(sfPos - sf::Vector2f{ element.sphere.radius, element.sphere.radius });
							drawables.circles.emplace_back(std::move(circle));
							drawables.orderedElements.push_back({
								.type = Drawables::Element::Circle,
								.index = drawables.circles.size() - 1,
								.height = height });
							break;
						}
						case ElementType::Box: {
							auto size = element.box.size;
							if (element.referenceSpace == DebugGraphicsComponent::ReferenceSpace::Object)
								size *= transform.boundingBox.size;
							sf::RectangleShape rectangle{ convertVector(size) };
							rectangle.setFillColor(color);
							rectangle.setPosition(convertVector(pos - size / 2.f));
							drawables.rectangles.emplace_back(std::move(rectangle));
							drawables.orderedElements.push_back({
								.type = Drawables::Element::Rectangle,
								.index = drawables.rectangles.size() - 1,
								.height = height });
							break;
						}
						default:
							static_assert(magic_enum::enum_count<ElementType>() == 3);
							kengine_assert_failed(r, "Unknown type");
							break;
					}
				}
			}

			std::ranges::sort(drawables.orderedElements, [](const Drawables::Element & lhs, const Drawables::Element & rhs) noexcept {
				return lhs.height < rhs.height;
			});

			for (const auto & element : drawables.orderedElements) {
				switch (element.type) {
					case Drawables::Element::Sprite: {
						renderTexture.draw(drawables.sprites[element.index]);
						break;
					}
					case Drawables::Element::Circle: {
						renderTexture.draw(drawables.circles[element.index]);
						break;
					}
					case Drawables::Element::Line: {
						renderTexture.draw(drawables.lines[element.index].vertices, 2, sf::PrimitiveType::Lines);
						break;
					}
					case Drawables::Element::Rectangle: {
						renderTexture.draw(drawables.rectangles[element.index]);
						break;
					}
					case Drawables::Element::Text: {
						// FreeType 2.11.0 (used by SFML as of 26/11/2021) is affected by an MSVC bug
						// Uncomment this as of the next FreeType version
						// renderTexture.draw(drawables.texts[element.index]);
						break;
					}
					default:
						kengine_assert_failed(r, "Unknown type");
						break;
				}
			}

			renderTexture.display();
		}

		std::optional<sf::Sprite> createEntitySprite(entt::entity e, const TransformComponent & transform, const GraphicsComponent & graphics) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto * texture = instanceHelper::tryGetModel<SFMLTextureComponent>({ r, e });
			if (texture == nullptr)
				return std::nullopt;

			sf::Sprite sprite(texture->texture);
			sprite.setColor(convertColor(graphics.color));
			sprite.setPosition(transform.boundingBox.position.x - transform.boundingBox.size.x / 2.f, transform.boundingBox.position.y - transform.boundingBox.size.y / 2.f);

			const auto textureSize = texture->texture.getSize();
			sprite.setScale(transform.boundingBox.size.x / textureSize.x, transform.boundingBox.size.y / textureSize.y);
			sprite.setRotation(transform.yaw);
			return sprite;
		}

		sf::Sprite createRenderTextureSprite(const sf::RenderTexture & renderTexture, const sf::RenderWindow & window, const ViewportComponent & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;

			sf::Sprite renderTextureSprite(renderTexture.getTexture());

			const auto screenSize = window.getSize();
			const auto box = cameraHelper::convertToScreenPercentage(viewport.boundingBox, { (float)screenSize.x, (float)screenSize.y }, viewport);
			renderTextureSprite.setPosition(screenSize.x * box.position.x, screenSize.y * box.position.y);

			const auto textureSize = renderTexture.getTexture().getSize();
			renderTextureSprite.setScale(screenSize.x / textureSize.x * box.size.x, screenSize.y / textureSize.y * box.size.y);

			return renderTextureSprite;
		}

		void createWindow(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & windowComp = r.get<WindowComponent>(e);
			kengine_logf(r, Log, "SFML", "Creating window '%s'", windowComp.name.c_str());


			auto & sfWindow = r.emplace<SFMLWindowComponent>(e, std::make_unique<sf::RenderWindow>(
				sf::VideoMode{ windowComp.size.x, windowComp.size.y },
				windowComp.name.c_str(),
				windowComp.fullscreen ? sf::Style::Fullscreen : sf::Style::Default
			));

			ImGui::SFML::Init(*sfWindow.window);

			r.emplace<ImGuiContextComponent>(e, ImGui::GetCurrentContext());
#if 0
			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;
#endif
			ImGui::SFML::Update(*sfWindow.window, deltaClock.restart());
		}

		static void createTexture(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & model = r.get<ModelComponent>(e);
			sf::Texture texture;
			if (texture.loadFromFile(model.file.c_str())) {
				kengine_logf(r, Log, "SFML", "Loaded texture for '%s'", model.file.c_str());
				r.emplace<SFMLTextureComponent>(e, std::move(texture));
			}
		}
	};
}

namespace kengine {
	void addSFMLSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<SFMLSystem>(e);
	}
}
