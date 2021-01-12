# [EntityCreator](EntityCreator.hpp)

```cpp
using EntityCreator = void(Entity &);

template<size_t Size>
using EntityCreatorFunctor = putils::function<EntityCreator, Size>;
```

Function used for creating `Entities`. Systems are often defined as functions returning an `EntityCreator`.