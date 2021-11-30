# [Mutex](Mutex.hpp)

Internal mutex type. Defaults to `std::shared_mutex`, unless `KENGINE_USE_SPINLOCK` was defined. In that case, [putils::SpinLock](../putils/SpinLock.md) is used instead.