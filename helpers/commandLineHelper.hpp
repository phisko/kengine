#pragma once

namespace kengine {
	KENGINE_CORE_EXPORT void createCommandLineEntity(int argc, const char ** argv) noexcept;

    template<typename T>
    T parseCommandLine() noexcept;
}

#include "commandLineHelper.inl"