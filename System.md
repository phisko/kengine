# [System](System.hpp)

A system holds game logic.

`Systems` are also [Modules](https://github.com/phisko/putils/blob/master/mediator/README.md), the `EntityManager` being their `Mediator`. This lets `Systems` easily communicate through `DataPackets`.

### Members

##### Definition

```cpp
template<typename CRTP, typename ...DataPackets>
class System;
```

A `System` is defined by its sub-type (see `CRTP`) and the list of `DataPackets` types it would like to receive.

##### RegisterEntity and RemoveEntity

If a `System` would like to be notified for each new `Entity` that is created, it can receive the [RegisterEntity](common/packets/RegisterEntity.hpp) and/or [RemoveEntity](common/packets/RemoveEntity.hpp) datapackets.

##### execute

```cpp
virtual void execute() {}
```
Runs the system's game logic. Called each frame.

##### getFrameRate

```cpp
virtual std::size_t getFrameRate() const noexcept { return 60; }
```
Returns how many times `execute` should be called each second.

Should return 0 if the framerate shouldn't be limited.

Note that this function is only evaluated upon loading a `System`, and systems with variable framerates are not supported at this point.

##### isPaused

```cpp
bool isPaused() const;
```

Returns whether the game is paused.

##### time

Each `System` has a `time` member that exposes the following functions:

```cpp
double getDeltaFrames() const;
```
Returns the number of frames since the last call to `execute`.

```cpp
putils::Timer::t_duration getDeltaTime() const;
```
Returns the time since the last call to `execute`.

```cpp
putils::Timer::t_duration getFixedDeltaTime() const;
```
Returns the expected time between two calls to execute, as determined by `getFrameRate`.

The `time` member structure is automatically managed by the `SystemManager`.
