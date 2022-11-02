#pragma once

namespace kengine {
	void createCommandLineEntity(int argc, const char ** argv) noexcept;

    template<typename T>
    T parseCommandLine() noexcept;
}

#include "commandLineHelper.inl"