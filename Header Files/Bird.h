#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Bird {
public:
    glm::vec3 position;
    float angle;
    float radius;
    float flapTimer;
    bool isFlapping;
    glm::quat birdRotation;

    Bird(glm::vec3 startPos, float startAngle, float startRadius);

    void Update(float deltaTime, glm::vec3 centerPoint, float arrivalThreshold);
};
