# [LogSystem](LogSystem.hpp)

`System` that receive [kengine::packets::Log](../packets/Log.hpp) packets and simply prints them to the standard output.

### Members

##### handle(const packets::Log &)

```cpp
void handle(const packets::Log &packet) const noexcept;
```
Handler for the `packets::Log` datapacket. See [Mediator](https://github.com/phiste/putils/blob/master/mediator/README.md) for clarification about how datapackets are transmitted.
