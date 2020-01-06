# [TypeHelper](TypeHelper.hpp)

Helper functions for manipulating "type Entities", i.e. `Entities` that hold information about a specific `Component` type.

## Members

### getTypeEntity

```cpp
template<typename T>
Entity getTypeEntity(EntityManager & em);
```

Returns the "type Entity" for `T`.