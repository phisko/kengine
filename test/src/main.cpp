//
// Created by naliwe on 6/24/16.
//

#include <stdlib.h>
#include <iostream>
#include "KTypes.hpp"
#include "Vector.hpp"

int main()
{
  Vector2<int> v(21, 42);
  std::cout << v << std::endl;
  std::cout << (unsigned) ComponentMask::_LAST << std::endl;
  return (0);
}
