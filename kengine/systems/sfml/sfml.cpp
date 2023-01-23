#include "sfml.hpp"

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
#include "putils/forward_to.hpp"
#include "putils/vector.hpp"

// kengine data
#include "kengine/data/camera.hpp"
#include "kengine/data/debug_graphics.hpp"
#include "kengine/data/graphics.hpp"
#include "kengine/data/imgui_context.hpp"
#include "kengine/data/input_buffer.hpp"
#include "kengine/data/model.hpp"
#include "kengine/data/transform.hpp"
#include "kengine/data/viewport.hpp"
#include "kengine/data/window.hpp"
#include "kengine/data/adjustable.hpp"
#include "kengine/data/imgui_scale.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/camera_helper.hpp"
#include "kengine/helpers/instance_helper.hpp"
#include "kengine/helpers/is_running.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// impl
#include "imgui-sfml/imgui-SFML.h"
#include "sfml_window.hpp"
#include "sfml_texture.hpp"

#ifndef KENGINE_MAX_VIEWPORTS
#define KENGINE_MAX_VIEWPORTS 8
#endif

namespace kengine::systems {
	template<typename PointType>
	struct vector_converter {
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
#define convertVector(vec) vector_converter<decltype(vec)>(vec)

	sf::Color convert_color(const putils::normalized_color & color) noexcept {
		const auto tmp = to_color(color);
		return { tmp.r, tmp.g, tmp.b, tmp.a };
	}

	struct sfml {
		entt::registry & r;
		putils::vector<entt::scoped_connection, 2> connections;

		sf::Clock delta_clock;
		data::input_buffer * input_buffer = nullptr;

		sfml(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/sfml");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			auto & scale = e.emplace<data::imgui_scale>();

			e.emplace<data::adjustable>() = {
				"ImGui",
				{
					{ "scale", &scale.scale },
				}
			};

			connections.emplace_back(r.on_construct<data::window>().connect<&sfml::create_window>(this));
			connections.emplace_back(r.on_construct<data::model>().connect<create_texture>());
		}

		~sfml() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Terminate/SFML", "Shutting down ImGui");
			ImGui::SFML::Shutdown();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "sfml");

			const auto sf_delta_time = delta_clock.restart();

			if (input_buffer == nullptr) {
				for (const auto & [e, buffer] : r.view<data::input_buffer>().each()) {
					input_buffer = &buffer;
					break;
				}
			}

			for (auto [window, sf_window] : r.view<data::sfml_window>().each()) {
				kengine_logf(r, verbose, "execute/sfml", "Processing window %zu", window);
				if (!update_window_state(window, sf_window))
					continue;
				// We process events after rendering, even though it's not the expected order, because
				// of how the SFML-ImGui binding handles input :(
				render(window, sf_window);
				process_events(window, *sf_window.window, sf_delta_time);
			}
		}

		bool update_window_state(entt::entity window_entity, data::sfml_window & sf_window_comp) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto * window_comp = r.try_get<data::window>(window_entity);
			if (window_comp == nullptr) {
				kengine_logf(r, verbose, "execute/sfml", "%zu: destroying window as data::window was removed", window_entity);
				r.remove<data::sfml_window>(window_entity);
				return false;
			}

			if (!sf_window_comp.window->isOpen()) {
				kengine_logf(r, verbose, "execute/sfml", "%zu: window was closed", window_entity);
				r.destroy(window_entity);
				return false;
			}

			sf_window_comp.window->setSize(convertVector(window_comp->size));
			return true;
		}

		void process_events(entt::entity window, sf::RenderWindow & sf_window, sf::Time delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			sf::Event event;
			while (sf_window.pollEvent(event)) {
				ImGui::SFML::ProcessEvent(sf_window, event);

				if (event.type == sf::Event::Closed)
					sf_window.close();

				process_input(window, event);
			}
			ImGui::SFML::Update(sf_window, delta_time);
		}

		putils::point2f previous_mouse_pos{ 0.f, 0.f };
		void process_input(entt::entity window, const sf::Event & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (input_buffer == nullptr)
				return;

			switch (e.type) {
				case sf::Event::KeyPressed:
				case sf::Event::KeyReleased: {
					if (ImGui::GetIO().WantCaptureKeyboard)
						break;

					input_buffer->keys.push_back(data::input_buffer::key_event{
						.window = window,
						.key = e.key.code,
						.pressed = e.type == sf::Event::KeyPressed,
					});
					break;
				}
				case sf::Event::MouseMoved: {
					if (ImGui::GetIO().WantCaptureMouse)
						break;

					const putils::point2f pos{ (float)e.mouseMove.x, (float)e.mouseMove.y };
					const putils::point2f rel = pos - previous_mouse_pos;
					previous_mouse_pos = pos;

					input_buffer->moves.push_back(data::input_buffer::mouse_move_event{
						.window = window,
						.pos = pos,
						.rel = rel,
					});
					break;
				}
				case sf::Event::MouseButtonPressed:
				case sf::Event::MouseButtonReleased: {
					if (ImGui::GetIO().WantCaptureMouse)
						break;

					input_buffer->clicks.push_back(data::input_buffer::click_event{
						.window = window,
						.pos = { (float)e.mouseButton.x, (float)e.mouseButton.y },
						.button = e.mouseButton.button,
						.pressed = e.type == sf::Event::MouseButtonPressed,
					});
					break;
				}
				case sf::Event::MouseWheelScrolled: {
					if (ImGui::GetIO().WantCaptureMouse)
						return;

					input_buffer->scrolls.push_back(data::input_buffer::mouse_scroll_event{
						.window = window,
						.xoffset = 0,
						.yoffset = e.mouseWheelScroll.delta,
						.pos = { (float)e.mouseWheelScroll.x, (float)e.mouseWheelScroll.y },
					});
					break;
				}
				default:
					// don't assert, this could be a joystick event or something
					break;
			}
		}

		void render(entt::entity window_entity, data::sfml_window & sf_window) noexcept {
			KENGINE_PROFILING_SCOPE;

			sf_window.window->clear();

			struct viewport_to_blit {
				const sf::RenderTexture * render_texture;
				const data::viewport * viewport;
			};
			putils::vector<viewport_to_blit, KENGINE_MAX_VIEWPORTS> to_blit;

			for (auto [e, cam, viewport] : r.view<data::camera, data::viewport>().each()) {
				if (viewport.window == entt::null) {
					kengine_logf(r, log, "systems/sfml", "Setting target window for data::viewport in %zu", e);
					viewport.window = window_entity;
				}
				else if (viewport.window != window_entity)
					continue;

				sf::RenderTexture * render_texture = (sf::RenderTexture *)viewport.texture;
				if (viewport.texture == data::viewport::INVALID_RENDER_TEXTURE) {
					render_texture = new sf::RenderTexture;
					render_texture->create(viewport.resolution.x, viewport.resolution.y);
					viewport.texture = (data::viewport::render_texture)render_texture;
				}

				render_texture->setView(sf::View{ convertVector(cam.frustum.position), convertVector(cam.frustum.size) });
				render_to_texture(*render_texture);
				to_blit.push_back(viewport_to_blit{ render_texture, &viewport });
			}

			std::ranges::sort(to_blit, [](const viewport_to_blit & lhs, const viewport_to_blit & rhs) noexcept {
				return lhs.viewport->z_order < rhs.viewport->z_order;
			});

			for (const auto & blit : to_blit) {
				auto render_texture_sprite = create_render_texture_sprite(*blit.render_texture, *sf_window.window, *blit.viewport);
				sf_window.window->draw(std::move(render_texture_sprite));
			}

			ImGui::SFML::Render(*sf_window.window);
			sf_window.window->display();
		}

		void render_to_texture(sf::RenderTexture & render_texture) noexcept {
			KENGINE_PROFILING_SCOPE;

			render_texture.clear();

			struct drawables {
				std::vector<sf::Sprite> sprites;
				std::vector<sf::CircleShape> circles;
				std::vector<sf::RectangleShape> rectangles;
				std::vector<sf::Text> texts;

				struct line {
					sf::Vertex vertices[2];
				};
				std::vector<line> lines;

				struct element {
					enum {
						sprite,
						circle,
						rectangle,
						text,
						line
					} type;
					size_t index;
					float height;
				};
				std::vector<element> ordered_elements;
			} drawables;

			for (const auto & [e, transform, graphics] : r.view<data::transform, data::graphics>().each()) {
				auto sprite = create_entity_sprite(e, transform, graphics);
				if (sprite != std::nullopt) {
					drawables.sprites.emplace_back(std::move(*sprite));
					drawables.ordered_elements.push_back({
						.type = drawables::element::sprite,
						.index = drawables.sprites.size() - 1,
						.height = transform.bounding_box.position.y,
					});
				}
			}

			for (const auto & [e, transform, debug] : r.view<data::transform, data::debug_graphics>().each()) {
				for (const auto & element : debug.elements) {
					const sf::Color color(convert_color(element.color));

					auto pos = element.pos;
					if (element.relative_to == data::debug_graphics::reference_space::object)
						pos += transform.bounding_box.position;

					const auto height = pos.z;

					using element_type = data::debug_graphics::element_type;
					switch (element.type) {
						case element_type::line: {
							drawables::line line;
							line.vertices[0].color = color;
							line.vertices[0].position = convertVector(element.pos); // lines are always in world space
							line.vertices[1].color = color;
							line.vertices[1].position = convertVector(element.line.end);
							drawables.lines.push_back(std::move(line));
							drawables.ordered_elements.push_back({
								.type = drawables::element::line,
								.index = drawables.lines.size() - 1,
								.height = height,
							});
							break;
						}
						case element_type::sphere: {
							sf::CircleShape circle(element.sphere.radius);
							circle.setFillColor(color);
							const sf::Vector2f sfPos = convertVector(pos);
							circle.setPosition(sfPos - sf::Vector2f{ element.sphere.radius, element.sphere.radius });
							drawables.circles.emplace_back(std::move(circle));
							drawables.ordered_elements.push_back({
								.type = drawables::element::circle,
								.index = drawables.circles.size() - 1,
								.height = height,
							});
							break;
						}
						case element_type::box: {
							auto size = element.box.size;
							if (element.relative_to == data::debug_graphics::reference_space::object)
								size *= transform.bounding_box.size;
							sf::RectangleShape rectangle{ convertVector(size) };
							rectangle.setFillColor(color);
							rectangle.setPosition(convertVector(pos - size / 2.f));
							drawables.rectangles.emplace_back(std::move(rectangle));
							drawables.ordered_elements.push_back({
								.type = drawables::element::rectangle,
								.index = drawables.rectangles.size() - 1,
								.height = height,
							});
							break;
						}
						default:
							static_assert(magic_enum::enum_count<element_type>() == 3);
							kengine_assert_failed(r, "Unknown type");
							break;
					}
				}
			}

			std::ranges::sort(drawables.ordered_elements, [](const drawables::element & lhs, const drawables::element & rhs) noexcept {
				return lhs.height < rhs.height;
			});

			for (const auto & element : drawables.ordered_elements) {
				switch (element.type) {
					case drawables::element::sprite: {
						render_texture.draw(drawables.sprites[element.index]);
						break;
					}
					case drawables::element::circle: {
						render_texture.draw(drawables.circles[element.index]);
						break;
					}
					case drawables::element::line: {
						render_texture.draw(drawables.lines[element.index].vertices, 2, sf::PrimitiveType::Lines);
						break;
					}
					case drawables::element::rectangle: {
						render_texture.draw(drawables.rectangles[element.index]);
						break;
					}
					case drawables::element::text: {
						// FreeType 2.11.0 (used by SFML as of 26/11/2021) is affected by an MSVC bug
						// Uncomment this as of the next FreeType version
						// render_texture.draw(drawables.texts[element.index]);
						break;
					}
					default:
						kengine_assert_failed(r, "Unknown type");
						break;
				}
			}

			render_texture.display();
		}

		std::optional<sf::Sprite> create_entity_sprite(entt::entity e, const data::transform & transform, const data::graphics & graphics) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto * texture = instance_helper::try_get_model<data::sfml_texture>({ r, e });
			if (texture == nullptr)
				return std::nullopt;

			sf::Sprite sprite(texture->texture);
			sprite.setColor(convert_color(graphics.color));
			sprite.setPosition(transform.bounding_box.position.x - transform.bounding_box.size.x / 2.f, transform.bounding_box.position.y - transform.bounding_box.size.y / 2.f);

			const auto texture_size = texture->texture.getSize();
			sprite.setScale(transform.bounding_box.size.x / texture_size.x, transform.bounding_box.size.y / texture_size.y);
			sprite.setRotation(transform.yaw);
			return sprite;
		}

		sf::Sprite create_render_texture_sprite(const sf::RenderTexture & render_texture, const sf::RenderWindow & window, const data::viewport & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;

			sf::Sprite render_texture_sprite(render_texture.getTexture());

			const auto screen_size = window.getSize();
			const auto box = camera_helper::convert_to_screen_percentage(viewport.bounding_box, { (float)screen_size.x, (float)screen_size.y }, viewport);
			render_texture_sprite.setPosition(screen_size.x * box.position.x, screen_size.y * box.position.y);

			const auto texture_size = render_texture.getTexture().getSize();
			render_texture_sprite.setScale(screen_size.x / texture_size.x * box.size.x, screen_size.y / texture_size.y * box.size.y);

			return render_texture_sprite;
		}

		void create_window(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & window_comp = r.get<data::window>(e);
			kengine_logf(r, log, "SFML", "Creating window '%s'", window_comp.name.c_str());

			auto & sf_window = r.emplace<data::sfml_window>(
				e,
				std::make_unique<sf::RenderWindow>(
					sf::VideoMode{ window_comp.size.x, window_comp.size.y },
					window_comp.name.c_str(),
					window_comp.fullscreen ? sf::Style::Fullscreen : sf::Style::Default
				)
			);

			ImGui::SFML::Init(*sf_window.window);

			r.emplace<data::imgui_context>(e, ImGui::GetCurrentContext());
#if 0
			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;
#endif
			ImGui::SFML::Update(*sf_window.window, delta_clock.restart());
		}

		static void create_texture(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & model = r.get<data::model>(e);
			sf::Texture texture;
			if (texture.loadFromFile(model.file.c_str())) {
				kengine_logf(r, log, "SFML", "Loaded texture for '%s'", model.file.c_str());
				r.emplace<data::sfml_texture>(e, std::move(texture));
			}
		}
	};

	entt::entity add_sfml(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<sfml>(e);
		return e;
	}
}
