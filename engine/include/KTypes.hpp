//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_KTYPES_HPP
# define KENGINE_KTYPES_HPP

enum class ComponentMask : unsigned
{
    Default   = 0,
    Graphical = (1 << 0),
    Kinematic = (1 << 1),
    Behaviour = (1 << 2),
    _LAST
};

#endif //KENGINE_KTYPES_HPP
