#pragma once

#include "sol.hpp"

struct LuaStateComponent {
	std::unique_ptr<sol::state> state = nullptr;
};