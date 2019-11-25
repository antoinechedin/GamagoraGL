//
// Created by achedin on 25/11/2019.
//

#include "Particle.h"

Particle::Particle(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &color) : position(
        position), velocity(velocity), color(color), age(0) {}
