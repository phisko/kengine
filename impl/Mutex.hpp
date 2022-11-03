#pragma once

#ifdef KENGINE_USE_SPINLOCK
// putils
# include "SpinLock.hpp"
#else
// stl
# include <shared_mutex>
#endif

namespace kengine::impl {
#ifdef KENGINE_USE_SPINLOCK
	using Mutex = putils::SpinLock;
	using ReadLock = Mutex::SharedLock;
	using WriteLock = Mutex::Lock;
#else
	using Mutex = std::shared_mutex;
	using ReadLock = std::shared_lock<Mutex>;
	using WriteLock = std::lock_guard<Mutex>;
#endif
}