#pragma once

#include <memory>
#include <GameObject.hpp>

class EntityFactory
{
public:
    virtual ~EntityFactory() = default;

public:
    virtual std::unique_ptr<GameObject> make(const std::string &type, const std::string &name) = 0;
};