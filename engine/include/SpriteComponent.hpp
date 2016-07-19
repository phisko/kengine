//
// File: [SpriteComponent.hpp]
//
// Author: flff.
// Contact: <Samy.kettani@gmail.com> (github.com/Flff)
//
// Created on 2016-07-15 16:43
//

#ifndef SPRITECOMPONENT_HPP_
# define SPRITECOMPONENT_HPP_

# include "IComponent.hpp"

class SpriteComponent : public IComponent
{
public:
  static ComponentMask Mask;

public:
  SpriteComponent(const std::string& name);
  SpriteComponent(const SpriteComponent& other);
  SpriteComponent(SpriteComponent&& other);
  SpriteComponent&	operator=(SpriteComponent other);
  SpriteComponent&	operator=(SpriteComponent&& other);
  virtual ~SpriteComponent() {}

public:
  bool	isUnique() const;

public:
  friend void swap(SpriteComponent& left, SpriteComponent& right);

public:
  virtual ComponentMask getMask() const override;
  virtual std::string	toString() const override;

private:
  std::string		_name;
  static const bool	_unique;
};

#endif //!SPRITECOMPONENT_HPP_
