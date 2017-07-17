//
// Created by naliwe on 7/15/16.
//

#pragma once

#include <vector>
#include "meta/type.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "Module.hpp"
#include "Timer.hpp"

namespace kengine
{
    class ISystem : public virtual putils::BaseModule
    {
    protected:
        ISystem() = default;

    public:
        virtual ~ISystem() = default;

    public:
        virtual void execute() {}
        virtual void registerGameObject(GameObject& go) {}
        virtual void removeGameObject(GameObject& go) {}
        virtual pmeta::type_index getType() const noexcept = 0;

        // Should return 0 if the system's framerate shouldn't be limited
        virtual std::size_t getFrameRate() const noexcept { return 60; }

        struct
        {
            friend class SystemManager;
        private:
            bool alwaysCall;
            putils::Timer timer;
            putils::Timer::t_duration deltaTime;
            putils::Timer::t_duration fixedDeltaTime;

            // Functions that may be called by System
        public:
            putils::Timer::t_duration getDeltaTime() const { return deltaTime; }
            putils::Timer::t_duration getFixedDeltaTime() const { return fixedDeltaTime; }
            double getDeltaFrames() const { return deltaTime / fixedDeltaTime; }
        } time;

    };
}
