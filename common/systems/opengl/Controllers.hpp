#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "components/LightComponent.hpp"
#include "packets/GBuffer.hpp"

namespace kengine {
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
					} else {
						glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
					}
				}
			};
		};
	}

	static auto ShadersController(kengine::EntityManager & em) {
		return [&](kengine::Entity & e) {
			e += kengine::ImGuiComponent([&] {
				if (ImGui::Begin("Shaders")) {
					if (ImGui::CollapsingHeader("GBuffer"))
						for (auto & [e, comp] : em.getEntities<kengine::GBufferShaderComponent>())
							ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

					if (ImGui::CollapsingHeader("Lighting"))
						for (auto & [e, comp] : em.getEntities<kengine::LightingShaderComponent>())
							ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);

					if (ImGui::CollapsingHeader("Post process"))
						for (auto & [e, comp] : em.getEntities<kengine::PostProcessShaderComponent>())
							ImGui::Checkbox(comp.shader->getName().c_str(), &comp.enabled);
				}
				ImGui::End();
			});
		};
	}

	static auto LightsDebugger(kengine::EntityManager & em) {
		return [&](kengine::Entity & e) {
			e += kengine::ImGuiComponent([&] {
				if (ImGui::Begin("Lights")) {
					bool first = true;
					if (ImGui::CollapsingHeader("Directional"))
						for (const auto &[e, light] : em.getEntities<DirLightComponent>()) {
							if (!first)
								ImGui::Separator();
							first = false;
							if (ImGui::Button(putils::string<64>("Remove##Directional") + e.id))
								em.removeEntity(e);
							ImGui::InputFloat3(putils::string<64>("Dir##Directional") + e.id, &light.direction.x);
							ImGui::InputFloat3(putils::string<64>("Color##Directional") + e.id, &light.color.x);
							ImGui::InputFloat(putils::string<64>("Ambient##Directional") + e.id, &light.ambientStrength);
							ImGui::InputFloat(putils::string<64>("Diffuse##Directional") + e.id, &light.diffuseStrength);
							ImGui::InputFloat(putils::string<64>("Specular##Directional") + e.id, &light.specularStrength);
						}

					first = true;
					if (ImGui::CollapsingHeader("Points")) {
						if (ImGui::Button("Add")) {
							em += [](kengine::Entity & e) {
								e += kengine::TransformComponent3f{};

								auto & light = e.attach<PointLightComponent>();
								light.diffuseStrength = 1.f;
								light.specularStrength = 5.f;
							};
						}

						for (const auto &[e, light, transform] : em.getEntities<PointLightComponent, kengine::TransformComponent3f>()) {
							if (!first)
								ImGui::Separator();
							first = false;
							if (ImGui::Button(putils::string<64>("Remove##Point") + e.id))
								em.removeEntity(e);
							ImGui::InputFloat3(putils::string<64>("Pos##Point") + e.id, transform.boundingBox.topLeft.raw);
							ImGui::InputFloat3(putils::string<64>("Color##Point") + e.id, &light.color.x);
							ImGui::InputFloat(putils::string<64>("Diffuse##Point") + e.id, &light.diffuseStrength);
							ImGui::InputFloat(putils::string<64>("Specular##Point") + e.id, &light.specularStrength);
						}
					}

					first = true;
					if (ImGui::CollapsingHeader("Spots")) {
						if (ImGui::Button("Add")) {
							em += [](kengine::Entity & e) {
								e += kengine::TransformComponent3f{};

								auto & light = e.attach<SpotLightComponent>();
								light.diffuseStrength = 1.f;
								light.specularStrength = 5.f;
							};
						}

						for (const auto &[e, light, transform] : em.getEntities<SpotLightComponent, kengine::TransformComponent3f>()) {
							if (!first)
								ImGui::Separator();
							first = false;
							if (ImGui::Button(putils::string<64>("Remove##Spot") + e.id))
								em.removeEntity(e);
							ImGui::InputFloat3(putils::string<64>("Pos##Spot") + e.id, transform.boundingBox.topLeft.raw);
							ImGui::InputFloat3(putils::string<64>("Dir##Spot") + e.id, &light.direction.x);
							ImGui::InputFloat3(putils::string<64>("Color##Spot") + e.id, &light.color.x);
							ImGui::InputFloat(putils::string<64>("Diffuse##Spot") + e.id, &light.diffuseStrength);
							ImGui::InputFloat(putils::string<64>("Specular##Spot") + e.id, &light.specularStrength);
							ImGui::InputFloat(putils::string<64>("Cutoff##Spot") + e.id, &light.cutOff);
							ImGui::InputFloat(putils::string<64>("Outer cutoff##Spot") + e.id, &light.outerCutOff);
						}
					}
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