#include "Bird.h"
#include <glm/gtx/quaternion.hpp>

Bird::Bird(glm::vec3 startPos, float startAngle, float startRadius)
    : position(startPos), angle(startAngle), radius(startRadius), birdRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) {}

void Bird::Update(float deltaTime, glm::vec3 centerPoint, float arrivalThreshold, glm::vec3 snakePosition, float distanceThreshold) {
    flapTimer--;

    if (flapTimer <= 0) {
        isFlapping = !isFlapping;
        flapTimer = 100 + rand() % (200 - 50 + 1) + 50;
    }

    if (isFlapping) {
        birdRotation = glm::quat(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else {
        birdRotation = glm::quat(0.0f, 1.0f, 0.0f, 0.0f);
    }

    float targetX, targetY, targetZ;

    if (glm::length(position - snakePosition) < 1.0f) {
        // Bird is within distance of the snake, pick up the snake
        isCarryingSnake = true;
    }

    if (isCarryingSnake) {
        // Fly straight up when carrying the snake
        position.y += deltaTime * 1.0f;

        // Check if the bird has flown high enough, then release the snake
        if (position.y >= centerPoint.y) {
            isCarryingSnake = false;
        }
    }
    else {
        if (glm::length(position - snakePosition) < distanceThreshold) {
            // Bird is within distance of the snake, fly towards the snake
            glm::vec3 directionToSnake = glm::normalize(snakePosition - position);
            position += directionToSnake * deltaTime * 1.0f;
            targetX = position.x;
            targetY = position.y;
            targetZ = position.z;
        }
        else {
            // Bird is not within distance of the snake, continue its normal behavior
            // Set the target position for this bird
            targetX = centerPoint.x + radius * std::cos(angle);
            targetZ = centerPoint.z + radius * std::sin(angle);
            targetY = centerPoint.y + 1.0f * std::sin(8.0f * angle);

            // Find the distance to the target
            float distanceToTarget = glm::length(position - glm::vec3(targetX, targetY, targetZ));

            // If the bird has reached the target, circle the point
            if (distanceToTarget < arrivalThreshold) {
                angle += deltaTime * 0.008; // Update the angle over time
                targetX = centerPoint.x + radius * std::cos(angle);
                targetZ = centerPoint.z + radius * std::sin(angle);
                targetY = centerPoint.y + 1.0f * std::sin(8.0f * angle);

                position.x = targetX;
                position.y = targetY;
                position.z = targetZ;
            }
            // If the bird has not reached their target, fly towards it
            else {
                glm::vec3 directionToTarget = glm::normalize(glm::vec3(targetX, targetY, targetZ) - position);
                position += directionToTarget * deltaTime * 1.0f;
            }
        }
    }
}
