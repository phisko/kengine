#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "data/LightComponent.hpp"
#include "functions/InitGBuffer.hpp"

namespace kengine {
	namespace Controllers {
		struct ShaderProfileComponent {
			float executionTime;
		};

		static auto ShaderController(EntityManager & em) {
			return [&](Entity & e) {
				auto & tool = e.attach<ImGuiToolComponent>();
				tool.name = "Shader controller";
				tool.enabled = false;

				e += ImGuiComponent([&] {
					if (!tool.enabled)
						return;

					if (ImGui::Begin("Shaders", &tool.enabled)) {
						static const auto displayShaders = [](const char * groupName, auto && shaders) {
							if (ImGui::CollapsingHeader(groupName))
								for (auto & [e, comp] : shaders) {
									ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);
									if (e.has<ShaderProfileComponent>()) {
										const auto & prof = e.get<ShaderProfileComponent>();
										ImGui::SameLine();
										ImGui::Text("%f", prof.executionTime);
									}
								}
						};

						displayShaders("GBuffer", em.getEntities<GBufferShaderComponent>());
						displayShaders("Lighting", em.getEntities<LightingShaderComponent>());
						displayShaders("Post lighting", em.getEntities<PostLightingShaderComponent>());
						displayShaders("Post process", em.getEntities<PostProcessShaderComponent>());
					}
					ImGui::End();
				});
			};
		}

		static auto GBufferDebugger(EntityManager & em, const functions::GBufferAttributeIterator & iterator) {
			return [&](Entity & e) {
				auto & tool = e.attach<ImGuiToolComponent>();
				tool.name = "GBuffer viewer";
				tool.enabled = false;

				e += ImGuiComponent([&] {
					if (!tool.enabled)
						return;
					
					if (ImGui::Begin("GBuffer viewer", &tool.enabled)) {
						for (const auto & [e, gbuffer] : em.getEntities<GBufferComponent>()) {
							if (ImGui::CollapsingHeader(putils::string<64>("%d", e.id))) {
								static bool * enabled = nullptr;
								if (enabled == nullptr)
									enabled = new bool[gbuffer.getTextureCount()];

								int i = 0;
								iterator([&](const char * name) {
									ImGui::Checkbox(name, &enabled[i]);
									if (!enabled[i]) {
										++i;
										return;
									}
									if (ImGui::Begin(putils::string<64>("%d [%s]", e.id, name), &enabled[i])) {
										const auto start = ImGui::GetWindowContentRegionMin();
										const auto end = ImGui::GetWindowContentRegionMax();
										ImGui::Image((ImTextureID)gbuffer.textures[i], { end.x - start.x, end.y - start.y }, { 0, 1 }, { 1, 0 });
									}
									ImGui::End();
									++i;
								});
							}
						}
					}
					ImGui::End();
				});
			};
		}
	}
}
