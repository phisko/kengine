//
// Created by naliwe on 6/24/16.
//

#include <stdlib.h>
#include <iostream>
#include <EntityManager.hpp>
#include "KTypes.hpp"

int main()
{
    EntityManager em;

    auto entity = em.createEntity<GameObject>("LeTest");

    std::cout << entity.toString() << std::endl;

    return (0);
}