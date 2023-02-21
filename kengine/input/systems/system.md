# [input](input.hpp)

System that reads the commands stored in an [input_buffer](../data/input_buffer.md) and forwards them to all entities with [input components](../data/input.md).

Graphics systems are responsible for filling the `input_buffer` with events.