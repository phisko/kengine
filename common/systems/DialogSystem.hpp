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
						_.x = .1f;
						_.z = .4f;
					}}
					{ pmeta_with(_.size) {
						_.x = .4f;
						_.z = .2f;
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
							_.x = .6f;
							_.z = i * .1f + .0125f;
						}}
						{ pmeta_with(_.size) {
							_.x = .3f;
							_.z = .075f;
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