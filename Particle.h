//
// Created by achedin on 25/11/2019.
//

#ifndef GAMAGORAGL_PARTICLE_H
#define GAMAGORAGL_PARTICLE_H


#include <glm/glm.hpp>

class Particle {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 color;
    float age;

    Particle(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &color);
};


#endif //GAMAGORAGL_PARTICLE_H
