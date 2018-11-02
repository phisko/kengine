#pragma once

#include "meta/type.hpp"
#include "Module.hpp"
#include "Timer.hpp"

namespace kengine {
    class ISystem : public virtual putils::BaseModule {
    protected:
        ISystem() = default;

    public:
        virtual ~ISystem() = default;

    public:
        virtual void execute() {}

        // Should return 0 if the system's framerate shouldn't be limited
        virtual std::size_t getFrameRate() const noexcept { return 60; }

        bool isPaused() const noexcept { return time.getDeltaFrames() == 0; }

        struct {
        public:
            putils::Timer::t_duration getDeltaTime() const { return deltaTime; }
            putils::Timer::t_duration getFixedDeltaTime() const { return fixedDeltaTime; }
            float getDeltaFrames() const { return deltaTime / fixedDeltaTime; }

            /*
             * Internals
             */

			friend class SystemManager;

        private:
            bool alwaysCall;
            putils::Timer timer;
            putils::Timer::t_duration deltaTime;
            putils::Timer::t_duration fixedDeltaTime;
            putils::Timer::t_clock::time_point lastCall;
        } time;

    public:
        virtual pmeta::type_index getType() const noexcept = 0;

    };
}
