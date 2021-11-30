#pragma once

#ifndef KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE
# define KENGINE_SCRIPT_SYSTEM_MAX_FUNCTION_SIZE 64
#endif

namespace kengine::scriptLanguageHelper {
	template<typename Func, typename Func2>
	void init(Func && registerFunction, Func2 && registerType) noexcept;

	template<typename T, typename Func, typename Func2>
	void registerComponent(Func && registerEntityMember, Func2 && registerFunction) noexcept;
}

#include "scriptLanguageHelper.inl"