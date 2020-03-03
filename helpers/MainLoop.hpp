#pragma once

namespace kengine {
	class EntityManager;

	namespace MainLoop {
		void run(EntityManager & em);

		namespace TimeModulated {
			void run(EntityManager & em);
		}
	}
}