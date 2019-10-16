#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "components/LightComponent.hpp"
#include "packets/GBuffer.hpp"

#ifndef KENGINE_NDEBUG

namespace kengine {
	namespace Controllers {
		struct Controller {
			const char * name;
			bool * enabled;
		};

		static std::vector<Controller> controllers;

		static auto MouseController(GLFWwindow * window) {
			return [window](kengine::Entity & e) {
				auto & comp = e.attach<kengine::InputComponent>();
				comp.onKey = [window](int key, bool pressed) {
					if (!pressed)
						return;
					if (key == GLFW_KEY_ENTER) {
						if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
							ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
						}
						else {
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
							ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
						}
					}
				};
			};
		}

		static auto ShaderController(kengine::EntityManager & em) {
			static bool display = false;
			controllers.push_back({ "Shader controller", &display });

			return [&](kengine::Entity & e) {
				e += kengine::ImGuiComponent([&] {
					if (!display)
						return;

					if (ImGui::Begin("Shaders", &display)) {
						if (ImGui::CollapsingHeader("GBuffer"))
							for (auto &[e, comp] : em.getEntities<kengine::GBufferShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

						if (ImGui::CollapsingHeader("Lighting"))
							for (auto &[e, comp] : em.getEntities<kengine::LightingShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

						if (ImGui::CollapsingHeader("Post lighting"))
							for (auto &[e, comp] : em.getEntities<kengine::PostLightingShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

						if (ImGui::CollapsingHeader("Post process"))
							for (auto &[e, comp] : em.getEntities<kengine::PostProcessShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);
					}
					ImGui::End();
				});
			};
		}

		static int TEXTURE_TO_DEBUG = -1;
		static auto TextureDebugger(kengine::EntityManager & em, const GBuffer & gBuffer, const packets::VertexDataAttributeIterator & iterator) {
			return [&](kengine::Entity & e) {
				e += kengine::ImGuiComponent([&] {
					if (ImGui::BeginMainMenuBar()) {
						if (ImGui::BeginMenu("Textures")) {
							if (ImGui::MenuItem("Disable"))
								TEXTURE_TO_DEBUG = -1;

							int i = 0;
							iterator.func([&](auto name) {
								if (ImGui::MenuItem(putils::string<64>("GBuffer %s", name)))
									TEXTURE_TO_DEBUG = gBuffer.textures[i];
								++i;
							});

							for (const auto &[e, comp] : em.getEntities<kengine::GBufferShaderComponent>())
								for (const auto & texture : comp.shader->texturesToDebug)
									if (ImGui::MenuItem(texture.name.c_str()))
										TEXTURE_TO_DEBUG = texture.id;

							for (const auto &[e, comp] : em.getEntities<kengine::LightingShaderComponent>())
								for (const auto & texture : comp.shader->texturesToDebug)
									if (ImGui::MenuItem(texture.name.c_str()))
										TEXTURE_TO_DEBUG = texture.id;

							for (const auto &[e, comp] : em.getEntities<kengine::PostProcessShaderComponent>())
								for (const auto & texture : comp.shader->texturesToDebug)
									if (ImGui::MenuItem(texture.name.c_str()))
										TEXTURE_TO_DEBUG = texture.id;

							ImGui::EndMenu();
						}
					}
					ImGui::EndMainMenuBar();
				});
			};
		}
	}
}

#endif