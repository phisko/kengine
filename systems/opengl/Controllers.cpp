#include "Controllers.hpp"
#include "kengine.hpp"

#include "data/ImGuiToolComponent.hpp"
#include "data/GBufferComponent.hpp"
#include "data/LightComponent.hpp"
#include "data/NameComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "functions/Execute.hpp"

#include "imgui.h"

namespace kengine::opengl {
	EntityCreator * ShaderController() noexcept {
		return [](Entity & e) {
			e += NameComponent{ "Shader controller" };

			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = false;

			e += functions::Execute{[&](float deltaTime) noexcept {
				if (!tool.enabled)
					return;

				if (ImGui::Begin("Shaders", &tool.enabled)) {
					static const auto displayShaders = [](const char * groupName, auto && shaders) {
						if (ImGui::CollapsingHeader(groupName))
							for (auto [e, shader, tag] : shaders) {
								ImGui::MenuItem(shader.shader->getName().c_str(), nullptr, &shader.enabled);

								const auto prof = e.tryGet<ShaderProfileComponent>();
								if (prof) {
									ImGui::SameLine();
									ImGui::Text("%f", prof->executionTime);
								}
							}
					};

					displayShaders("GBuffer", entities.with<ShaderComponent, GBufferShaderComponent>());
					displayShaders("Lighting", entities.with<ShaderComponent, LightingShaderComponent>());
					displayShaders("Post lighting", entities.with<ShaderComponent, PostLightingShaderComponent>());
					displayShaders("Post process", entities.with<ShaderComponent, PostProcessShaderComponent>());
				}
				ImGui::End();
			}};
		};
	}

	EntityCreatorFunctor<64> GBufferDebugger(const functions::GBufferAttributeIterator & iterator) noexcept {
		return [&](Entity & debugger) noexcept {
			debugger += NameComponent{ "GBuffer viewer" };

			auto & tool = debugger.attach<ImGuiToolComponent>();
			tool.enabled = false;

			debugger += functions::Execute{[&](float deltaTime) {
				if (!tool.enabled)
					return;

				if (ImGui::Begin("GBuffer viewer", &tool.enabled)) {
					for (const auto & [e, gbuffer] : entities.with<GBufferComponent>()) {
						if (ImGui::CollapsingHeader(putils::string<64>("%d", e.id))) {
							static bool * enabled = nullptr;
							if (enabled == nullptr)
								enabled = new bool[gbuffer.getTextureCount()];

							int i = 0;
							iterator([&i, &e, &gbuffer](const char * name) noexcept {
								ImGui::Checkbox(name, &enabled[i]);
								if (!enabled[i]) {
									++i;
									return;
								}
								if (ImGui::Begin(putils::string<64>("%d [%s]", e.id, name), &enabled[i])) {
									const auto start = ImGui::GetWindowContentRegionMin();
									const auto end = ImGui::GetWindowContentRegionMax();
									ImGui::Image((ImTextureID)gbuffer.textures[i].get(), { end.x - start.x, end.y - start.y }, { 0, 1 }, { 1, 0 });
								}
								ImGui::End();
								++i;
							});
						}
					}
				}
				ImGui::End();
			} };
		};
	}
}
