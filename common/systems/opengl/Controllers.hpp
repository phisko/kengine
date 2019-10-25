#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "components/LightComponent.hpp"
#include "packets/GBuffer.hpp"

#ifndef KENGINE_NDEBUG

namespace kengine {
	namespace Controllers {
		static auto ShaderController(EntityManager & em) {
			return [&](Entity & e) {
				auto & tool = e.attach<ImGuiToolComponent>();
				tool.name = "Shader controller";
				tool.enabled = false;

				e += ImGuiComponent([&] {
					if (!tool.enabled)
						return;

					if (ImGui::Begin("Shaders", &tool.enabled)) {
						if (ImGui::CollapsingHeader("GBuffer"))
							for (auto &[e, comp] : em.getEntities<GBufferShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

						if (ImGui::CollapsingHeader("Lighting"))
							for (auto &[e, comp] : em.getEntities<LightingShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

						if (ImGui::CollapsingHeader("Post lighting"))
							for (auto &[e, comp] : em.getEntities<PostLightingShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

						if (ImGui::CollapsingHeader("Post process"))
							for (auto &[e, comp] : em.getEntities<PostProcessShaderComponent>())
								ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);
					}
					ImGui::End();
				});
			};
		}

		static int TEXTURE_TO_DEBUG = -1;
		static auto TextureDebugger(EntityManager & em, const GBuffer & gBuffer, const packets::VertexDataAttributeIterator & iterator) {
			return [&](Entity & e) {
				e += ImGuiComponent([&] {
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

							for (const auto &[e, comp] : em.getEntities<GBufferShaderComponent>())
								for (const auto & texture : comp.shader->texturesToDebug)
									if (ImGui::MenuItem(texture.name.c_str()))
										TEXTURE_TO_DEBUG = texture.id;

							for (const auto &[e, comp] : em.getEntities<LightingShaderComponent>())
								for (const auto & texture : comp.shader->texturesToDebug)
									if (ImGui::MenuItem(texture.name.c_str()))
										TEXTURE_TO_DEBUG = texture.id;

							for (const auto &[e, comp] : em.getEntities<PostProcessShaderComponent>())
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