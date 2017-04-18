#pragma once

#include <chrono>
#include <utility>

namespace putils
{
    //
    // Timer class
    //
    class Timer
    {
    public:
        using t_duration = std::chrono::duration<double>;
        using t_clock = std::chrono::system_clock;

        // Constructors
    public:
        template<typename Duration>
        Timer(Duration &&duration) noexcept
                : _duration(std::forward<Duration>(duration)), _start(t_clock::now()) {}

        // Coplien
    public:
        Timer(const Timer &other) noexcept = default;

        Timer &operator=(const Timer &other) noexcept = default;

        // Move
    public:
        Timer(Timer &&other) noexcept = default;

        Timer &operator=(Timer &&other) noexcept = default;

        // Is the timer ringing
    public:
        bool isDone() const noexcept { return t_clock::now() - _start >= _duration; }

        // How long ago did I start the timer
    public:
        t_duration getTimeSinceStart() const noexcept { return t_clock::now() - _start; }

        // How long until it rings
    public:
        t_duration getTimeLeft() const noexcept { return _start + _duration - t_clock::now(); }

        // Restart the timer
    public:
        void restart() noexcept { _start = t_clock::now(); }

        // Duration getters and setters
    public:
        const t_duration &getDuration() const noexcept { return _duration; }

        template<typename Duration>
        void setDuration(Duration &&duration) noexcept { _duration = duration; }

        void setDuration(double duration) noexcept { _duration = t_duration(duration); }
        //template<>
        //void	setDuration<double>(double &duration) noexcept { _duration = t_duration(duration); }
        //void	setDuration(const t_duration &duration) noexcept { _duration = duration; }

        // Attributes
    private:
        t_duration _duration;
        t_clock::time_point _start;
    };
}
