# putils
Phiste's C++ Utilities

## Directory
The "Directory" class family is an abstraction to directories on Unix and Windows. The `Directory` type is an OS-compatible version of `ADirectory`.

## Observable
A minimalistic (but greatly sufficient) implementation of the Observer pattern. Observers are simply `std::function<void()>`. Lambdas can therefore be used as observers, and store any necessary context through capture-lists.
Any class inheriting from `Observable` can have any number of "observers", and notify them by calling `changed()`.

## RAII
A class template able to perform RAII on any type. `RAII<T>` is constructed with a `T` and an `std::function<void(T)>` that will be called at destruction time.
Automatic conversions from `RAII<T>` to `T` exist to greatly simplify usage.

For instance, to securely use a C socket:
```
auto socket = RAII<int>{ socket(...), [](int sock) { close(sock); } };
```
For an OpenSSL object:
```
auto ctx = RAII<SSL*> { nullptr, [](SSL *ssl) { SSL_free(ssl); } };
```
Movable, but not copyable.

## Timer
Simple timer class based on std::chrono.

## EventManager
A minimalistic event manager: events are `Observables` given a name. Anyone can trigger an event by indicating that it `changed()`, which will call anyone observing the event.

## Serializable
Mix-in class letting any derived class serialize its attributes with great simplicity. An example is the simplest way to show you:
```
#include "Serializable.hpp"

class Example : public putils::Serializable<Example>
{
  private:
    std::string _name;
    int _i;

  public:
    Example(const std::string &name, int i)
      : Serializable(&Example::_name, &Example::_i),
	_name(name), _i(i)
    {}
};

int main()
{
  Example e("Bob", 42);

  e.serialize(std::cout);
}
```
As you can see, `Example` constructs its `Serializable` base by giving it pointers to its members. When `e.serialize(std::cout)` is called, all those members get serialized to stdout.

## AbstractFactory
Template class for an abstract factory able to create any type in a provided typelist.
Given an `AbstractFactory<TList...>`, users can call `make<T>()` to create an object of type T (or any type derived from it, as determined by the concrete factory implementation).

### Example code
In this example, "ConcreteFactory" can create `ints` and `doubles`, which isn't particularly useful. However, you can imagine how this could be useful if ConcreteFactory could create concrete versions of GUI elements, for instance.
```
#include <iostream>
#include "AbstractFactory.hpp"

class ConcreteFactory : public putils::AbstractFactory<int, double>
{
  public:
    int *makeImpl(pmeta::type<int>) noexcept override
    {
      std::cout << "int" << std::endl;
      return new int;
    }

    double *makeImpl(pmeta::type<double>) noexcept override
    {
      std::cout << "double" << std::endl;
      return new double;
    }
};

int main()
{
  ConcreteFactory cf;

  cf.make<int>();
  cf.make<double>();
}
```
