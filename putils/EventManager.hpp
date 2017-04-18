#pragma once

#include <unordered_map>
#include "putils/Observable.hpp"

namespace putils
{
	//
	// Observable event
	// To subscrive to an event, simply observe it. To throw an event, use eventManager["eventId"].changed()
	//
	using Event = Observable<>;

	//
	// An event manager simply maps strings to observable elements
	//
	using EventManager = std::unordered_map<std::string, Event>;
}