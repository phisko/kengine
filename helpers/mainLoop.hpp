#pragma once

namespace kengine {
	class EntityManager;

	namespace mainLoop {
		void run(EntityManager & em);

		namespace timeModulated {
			void run(EntityManager & em);
		}
	}
}