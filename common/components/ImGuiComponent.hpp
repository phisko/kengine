#pragma once

#include <functional>
#include "SerializableComponent.hpp"
#include "EntityManager.hpp"

struct ImGuiContext;
extern ImGuiContext * GImGui;

namespace kengine {
	class ImGuiComponent : public kengine::SerializableComponent<ImGuiComponent> {
	public:
		static kengine::GameObject & create(const std::string & name, kengine::EntityManager & em, const std::function<void()> & display) {
			return em.createEntity<kengine::GameObject>(name, [&display](kengine::GameObject & go) {
				go.attachComponent<kengine::ImGuiComponent>([display](auto && context) {
					setupImGuiContext(context);
					display();
				});
			});
		}

	public:
		ImGuiComponent(const std::function<void(void *)> & display = nullptr) : display(display) {}

	public:
		std::function<void(void * context)> display;

		static void setupImGuiContext(void * context) {
			GImGui = (decltype(GImGui))context;
		}

		/*
		 * Reflectible
		 */

		pmeta_get_class_name(ImGuiComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ImGuiComponent::display)
		);
		pmeta_get_methods(
			pmeta_reflectible_attribute(&ImGuiComponent::setupImGuiContext)
		);
		pmeta_get_parents();
	};
}
