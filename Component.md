# Component

Holds information about a certain property of a [GameObject](GameObject.md).

`Components` are also [Modules](putils/mediator/README.md), the `GameObject` being the `Mediator`. This allows the various `Components` of a `GameObject` to have a simple means of communication.

### Members

##### Definition

```
template<typename CRTP, typename ...DataPackets>
class Component;
```

A `Component` is defined by its sub-type (see `CRTP`) and the list of `DataPackets` it would like to receive.

### Virtual members

##### toString

```
virtual std::string toString() const = 0;

friend std::ostream &operator<<(std::ostream &s, const kengine::IComponent &obj)
{
    s << obj.toString();
    return s;
}
```

For [GameObjects](GameObject.md) to be serialized, each `Component` must define a `toString` method.

This method should return a JSON-formatted string, which will fit in with the rest of the `GameObject` serialization.
