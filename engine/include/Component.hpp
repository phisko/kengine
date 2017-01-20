//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_COMPONENT_HPP
# define KENGINE_COMPONENT_HPP

# include <string>
# include "IComponent.hpp"

class Component : public IComponent
{
 public:
  static ComponentMask Mask;
 public:
  Component(std::string const& name)
    : _name(name)
  { }

  Component(Component const& other);
  Component& operator=(Component other);
  Component& operator=(Component&& other);

  virtual ~Component()
  { }

 public:
  bool isUnique() const;

 public:
  friend void swap(Component& left, Component& right);

 public:
  virtual ComponentMask getMask() const override;
  virtual std::string toString() const override;

  std::string const& get_name() const
  { return _name; }

 private:
  std::string       _name;
  static const bool _unique;
};

#endif //KENGINE_COMPONENT_HPP
