#pragma once

#include <unordered_set>
#include "System.hpp"
#include "EntityManager.hpp"
#include "components/DialogComponent.hpp"
#include "components/GUIComponent.hpp"
#include "with.hpp"

namespace kengine {
	class DialogSystem : public kengine::System<DialogSystem> {
	public:
		DialogSystem(kengine::EntityManager & em) : _em(em) {
		}

	public:
		void execute() noexcept final {
			const auto & objects = _em.getGameObjects<kengine::DialogComponent>();
			auto & mainText = _em.createOrAttach<kengine::GUIComponent>("dialog-mainText");

			if (objects.empty()) {
				_em.disableEntity(mainText);
				for (const auto answer : _answers)
					_em.disableEntity(*answer);
				_answers.clear();
				return;
			}

			_em.enableEntity(mainText);

			auto & dialog = *objects.back();
			const auto & comp = dialog.getComponent<kengine::DialogComponent>();

			{ pmeta_with(mainText.getComponent<kengine::GUIComponent>()) {
				_.guiType = kengine::GUIComponent::Text;
				_.text = comp.mainText;

				{ pmeta_with(_.boundingBox) {
					{ pmeta_with(_.topLeft) {
						_.x = 0.1;
						_.z = 0.4;
					}}
					{ pmeta_with(_.size) {
						_.x = 0.4;
						_.z = 0.2;
					}}
				}}
				_.changed();
			}}

			int i = 0;
			for (const auto & option : comp.options) {
				auto & answer = _em.createOrAttach<kengine::GUIComponent>(putils::concat("dialog-answer", i));
				_em.enableEntity(answer);
				_answers.emplace(&answer);

				{ pmeta_with(answer.getComponent<kengine::GUIComponent>()) {
					_.guiType = kengine::GUIComponent::Button;
					_.text = option.text;

					_.onClick = [this, &option, &dialog, &mainText]{
						option.onClick();
						_em.removeEntity(dialog);
					};

					{ pmeta_with(_.boundingBox) {
						{ pmeta_with(_.topLeft) {
							_.x = 0.6;
							_.z = i * 0.1 + 0.0125;
						}}
						{ pmeta_with(_.size) {
							_.x = 0.3;
							_.z = 0.075;
						}}
					}}
					_.changed();
				}}
				++i;
			}
		}

	private:
		std::unordered_set<kengine::GameObject *> _answers;
		kengine::EntityManager & _em;
	};
}