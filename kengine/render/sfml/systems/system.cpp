#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// imgui-sfml
#include <imgui-SFML.h>

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

// kengine
#include "kengine/adjustable/data/values.hpp"
#include "kengine/core/data/transform.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/data/context.hpp"
#include "kengine/imgui/data/scale.hpp"
#include "kengine/input/data/buffer.hpp"
#include "kengine/model/helpers/try_get.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/is_running.hpp"
#include "kengine/render/data/asset.hpp"
#include "kengine/render/data/camera.hpp"
#include "kengine/render/data/debug_graphics.hpp"
#include "kengine/render/data/drawable.hpp"
#include "kengine/render/data/viewport.hpp"
#include "kengine/render/data/window.hpp"
#include "kengine/render/helpers/convert_to_screen_percentage.hpp"
#include "kengine/render/sfml/data/texture.hpp"
#include "kengine/render/sfml/data/window.hpp"

#ifndef KENGINE_MAX_VIEWPORTS
#define KENGINE_MAX_VIEWPORTS 8
#endif

namespace kengine::render::sfml {
	static constexpr auto log_category = "render_sfml";

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

	struct system {
		entt::registry & r;
		putils::vector<entt::scoped_connection, 2> connections;

		sf::Clock delta_clock;
		input::buffer * input_buffer = nullptr;

		struct processed_window {};
		kengine::new_entity_processor<processed_window, render::window> window_processor{ r, putils_forward_to_this(create_window) };

		struct processed_model {};
		kengine::new_entity_processor<processed_model, render::asset> model_processor{ r, putils_forward_to_this(create_texture) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
			auto & scale = e.emplace<imgui::scale>();
			e.emplace<adjustable::values>() = {
				"ImGui",
				{
					{ "scale", &scale.modifier },
				}
			};

			window_processor.process();
			model_processor.process();
		}

		~system() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Shutting down");
			ImGui::SFML::Shutdown();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			window_processor.process();
			model_processor.process();

			const auto sf_delta_time = delta_clock.restart();

			if (input_buffer == nullptr) {
				kengine_log(r, verbose, log_category, "Finding input buffer");
				for (const auto & [e, buffer] : r.view<input::buffer>().each()) {
					input_buffer = &buffer;
					break;
				}
			}

			kengine_log(r, very_verbose, log_category, "Processing windows");
			for (auto [window, sf_window] : r.view<sfml::window>().each()) {
				kengine_logf(r, very_verbose, log_category, "Processing window {}", window);
				if (!update_window_state(window, sf_window))
					continue;
				// We process events after rendering, even though it's not the expected order, because
				// of how the SFML-ImGui binding handles input :(
				render(window, sf_window);
				process_events(window, *sf_window.ptr, sf_delta_time);
			}
		}

		bool update_window_state(entt::entity window_entity, window & sf_window_comp) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Updating window state for {}", window_entity);

			const auto * window_comp = r.try_get<render::window>(window_entity);
			if (window_comp == nullptr) {
				kengine_logf(r, verbose, log_category, "Destroying window {} as render::window was removed", window_entity);
				r.remove<window>(window_entity);
				return false;
			}

			if (!sf_window_comp.ptr->isOpen()) {
				kengine_logf(r, verbose, log_category, "Destroying {} because window was closed", window_entity);
				r.destroy(window_entity);
				return false;
			}

			sf_window_comp.ptr->setSize(convertVector(window_comp->size));
			return true;
		}

		void process_events(entt::entity window, sf::RenderWindow & sf_window, sf::Time delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Processing events for {}", window);

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
			kengine_logf(r, very_verbose, log_category, "Processing input event for {}", window);

			if (input_buffer == nullptr) {
				kengine_log(r, verbose, log_category, "No input buffer, cannot process event");
				return;
			}

			switch (e.type) {
				case sf::Event::KeyPressed:
				case sf::Event::KeyReleased: {
					if (ImGui::GetIO().WantCaptureKeyboard)
						break;

					input_buffer->keys.push_back(input::buffer::key_event{
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

					input_buffer->moves.push_back(input::buffer::mouse_move_event{
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

					input_buffer->clicks.push_back(input::buffer::click_event{
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

					input_buffer->scrolls.push_back(input::buffer::mouse_scroll_event{
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

		void render(entt::entity window_entity, window & sf_window) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Rendering to {}", window_entity);

			sf_window.ptr->clear();

			struct viewport_to_blit {
				const sf::RenderTexture * render_texture;
				const viewport * ptr;
			};
			putils::vector<viewport_to_blit, KENGINE_MAX_VIEWPORTS> to_blit;

			for (auto [e, cam, viewport] : r.view<camera, viewport>().each()) {
				kengine_logf(r, very_verbose, log_category, "Rendering to camera {}", e);

				if (viewport.window == entt::null) {
					kengine_logf(r, verbose, log_category, "Setting target window for viewport in {}", e);
					viewport.window = window_entity;
				}
				else if (viewport.window != window_entity) {
					kengine_logf(r, very_verbose, log_category, "Skipping camera {} because its viewport ({}) doesn't match", e, viewport.window);
					continue;
				}

				sf::RenderTexture * render_texture = (sf::RenderTexture *)viewport.texture;
				if (viewport.texture == viewport::INVALID_RENDER_TEXTURE) {
					kengine_logf(r, verbose, log_category, "Creating render texture for camera {}", e);
					render_texture = new sf::RenderTexture;
					render_texture->create(viewport.resolution.x, viewport.resolution.y);
					viewport.texture = (viewport::render_texture)render_texture;
				}

				render_texture->setView(sf::View{ convertVector(cam.frustum.position), convertVector(cam.frustum.size) });
				render_to_texture(*render_texture);
				to_blit.push_back(viewport_to_blit{ render_texture, &viewport });
			}

			std::ranges::sort(to_blit, [](const viewport_to_blit & lhs, const viewport_to_blit & rhs) noexcept {
				return lhs.ptr->z_order < rhs.ptr->z_order;
			});

			for (const auto & blit : to_blit) {
				auto render_texture_sprite = create_render_texture_sprite(*blit.render_texture, *sf_window.ptr, *blit.ptr);
				sf_window.ptr->draw(std::move(render_texture_sprite));
			}

			ImGui::SFML::Render(*sf_window.ptr);
			sf_window.ptr->display();
		}

		void render_to_texture(sf::RenderTexture & render_texture) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Rendering to texture");

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

			kengine_log(r, very_verbose, log_category, "Queueing sprites");
			for (const auto & [e, transform, drawable] : r.view<core::transform, render::drawable>().each()) {
				auto sprite = create_entity_sprite(e, transform, drawable);
				if (sprite != std::nullopt) {
					kengine_logf(r, very_verbose, log_category, "Queueing sprite for {}", e);
					drawables.sprites.emplace_back(std::move(*sprite));
					drawables.ordered_elements.push_back({
						.type = drawables::element::sprite,
						.index = drawables.sprites.size() - 1,
						.height = transform.bounding_box.position.y,
					});
				}
			}

			kengine_log(r, very_verbose, log_category, "Queueing debug graphics");
			for (const auto & [e, transform, debug] : r.view<core::transform, debug_graphics>().each()) {
				kengine_logf(r, very_verbose, log_category, "Queueing debug graphics for {}", e);
				for (const auto & element : debug.elements) {
					const sf::Color color(convert_color(element.color));

					auto pos = element.pos;
					if (element.relative_to == debug_graphics::reference_space::object)
						pos += transform.bounding_box.position;

					const auto height = pos.z;

					using element_type = debug_graphics::element_type;
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
							if (element.relative_to == debug_graphics::reference_space::object)
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

			kengine_log(r, very_verbose, log_category, "Sorting drawables");
			std::ranges::sort(drawables.ordered_elements, [](const drawables::element & lhs, const drawables::element & rhs) noexcept {
				return lhs.height < rhs.height;
			});

			kengine_log(r, very_verbose, log_category, "Drawing drawables");
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

		std::optional<sf::Sprite> create_entity_sprite(entt::entity e, const core::transform & transform, const render::drawable & drawable) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Creating entity sprite for {}", e);

			const auto * texture = model::try_get<sfml::texture>({ r, e });
			if (texture == nullptr) {
				kengine_logf(r, warning, log_category, "Failed to find sfml_texture in {}'s model", e);
				return std::nullopt;
			}

			sf::Sprite sprite(texture->value);
			sprite.setColor(convert_color(drawable.color));
			sprite.setPosition(transform.bounding_box.position.x - transform.bounding_box.size.x / 2.f, transform.bounding_box.position.y - transform.bounding_box.size.y / 2.f);

			const auto texture_size = texture->value.getSize();
			sprite.setScale(transform.bounding_box.size.x / texture_size.x, transform.bounding_box.size.y / texture_size.y);
			sprite.setRotation(transform.yaw);
			return sprite;
		}

		sf::Sprite create_render_texture_sprite(const sf::RenderTexture & render_texture, const sf::RenderWindow & window, const viewport & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Creating render texture sprite");

			sf::Sprite render_texture_sprite(render_texture.getTexture());

			const auto screen_size = window.getSize();
			const auto box = convert_to_screen_percentage(viewport.bounding_box, { (float)screen_size.x, (float)screen_size.y }, viewport);
			render_texture_sprite.setPosition(screen_size.x * box.position.x, screen_size.y * box.position.y);

			const auto texture_size = render_texture.getTexture().getSize();
			render_texture_sprite.setScale(screen_size.x / texture_size.x * box.size.x, screen_size.y / texture_size.y * box.size.y);

			return render_texture_sprite;
		}

		void create_window(entt::entity e, const render::window & window_comp) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, log_category, "Creating window for {} ({})", e, window_comp.name);

			auto & sf_window = r.emplace<window>(
				e,
				std::make_unique<sf::RenderWindow>(
					sf::VideoMode{ window_comp.size.x, window_comp.size.y },
					window_comp.name.c_str(),
					window_comp.fullscreen ? sf::Style::Fullscreen : sf::Style::Default
				)
			);

			ImGui::SFML::Init(*sf_window.ptr);

			r.emplace<imgui::context>(e, ImGui::GetCurrentContext());
#if 0
			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;
#endif
			ImGui::SFML::Update(*sf_window.ptr, delta_clock.restart());
		}

		void create_texture(entt::entity e, const render::asset & asset) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Attempting to load texture for {} ({})", e, asset.file);

			sf::Texture texture;
			if (texture.loadFromFile(asset.file.c_str())) {
				kengine_logf(r, log, log_category, "Loaded texture for '{}'", asset.file);
				r.emplace<sfml::texture>(e, std::move(texture));
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed_model,
		system::processed_window,
		sfml::window,
		sfml::texture
	)
}
