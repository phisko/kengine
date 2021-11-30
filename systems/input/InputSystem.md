# [InputSystem](InputSystem.hpp)

`System` that reads the commands stored in an [InputBufferComponent](../components/data/InputBufferComponent.md) and forwards them to all `Entities` with [InputComponents](../components/data/InputComponent.md).

Graphics systems are responsible for filling the `InputBufferComponent` with input events.