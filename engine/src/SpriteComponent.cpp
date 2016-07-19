//
// File: [SpriteComponent.cpp]
//
// Author: flff.
// Contact: <Samy.kettani@gmail.com> (github.com/Flff)
//
// Created on 2016-07-18 11:18
//

#include <sstream>
#include "SpriteComponent.hpp"

ComponentMask	SpriteComponent::Mask = ComponentMask::Graphical;
const bool	SpriteComponent::_unique = false;

SpriteComponent::SpriteComponent(const std::string& name) :
    _name(name)
{}

SpriteComponent::SpriteComponent(const SpriteComponent& other)
{
  if (this != &other)
    {
      _name = other._name;
    }
}

SpriteComponent::SpriteComponent(SpriteComponent&& other)
{
  swap(*this, other);
}

SpriteComponent&	SpriteComponent::operator=(SpriteComponent other)
{
  swap(*this, other);

  return *this;
}

SpriteComponent&	SpriteComponent::operator=(SpriteComponent&& other)
{
  swap(*this, other);

  return *this;
}

bool	SpriteComponent::isUnique() const
{
  return _unique;
}

void	swap(SpriteComponent& left, SpriteComponent& right)
{
  using std::swap;

  swap(left._name, right._name);
}

ComponentMask	SpriteComponent::getMask() const
{
  return Mask;
}

std::string	SpriteComponent::toString() const
{
  std::stringstream ss;

  ss << "Component:\tSprite:\t" << _name << std::endl;
  return ss.str();
}
