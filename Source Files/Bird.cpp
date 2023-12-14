#include "Bird.h"
#include <glm/gtx/quaternion.hpp>

Bird::Bird(glm::vec3 startPos, float startAngle, float startRadius)
    : position(startPos), angle(startAngle), radius(startRadius), birdRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {}

void Bird::Update(float deltaTime, glm::vec3 centerPoint, float arrivalThreshold) {
    flapTimer--;

    if (flapTimer <= 0) {
        isFlapping = !isFlapping;
        flapTimer = 300 + rand() % (400 - 50 + 1) + 50;
    }

    if (isFlapping) {
        birdRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        birdRotation = glm::quat(0.0f, 1.0f, 0.0f, 0.0f);
    }

    float targetX = centerPoint.x + radius * std::cos(angle);
    float targetZ = centerPoint.z + radius * std::sin(angle);
    float targetY = centerPoint.y + 1.0f * std::sin(8.0f * angle);

    glm::vec3 target(targetX, targetY, targetZ);
    float distanceToTarget = glm::length(position - target);

    if (distanceToTarget < arrivalThreshold) {
        angle += deltaTime * 0.008;
        targetX = centerPoint.x + radius * std::cos(angle);
        targetZ = centerPoint.z + radius * std::sin(angle);
        targetY = centerPoint.y + 1.0f * std::sin(8.0f * angle);

        position.x = targetX;
        position.y = targetY;
        position.z = targetZ;
    }
    else {
        glm::vec3 directionToTarget = glm::normalize(target - position);
        position += directionToTarget * deltaTime * 1.0f;
    }
}
