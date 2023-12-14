#pragma once

#include <glm/glm.hpp>

class CelestialBody {
public:
    glm::vec3 position;
    float angle;  // Angle in radians
    float radius; // Radius of the circular motion

    CelestialBody(glm::vec3 startPos, float startAngle, float startRadius, float phaseDifference = 0.0f);

    void Update(float deltaTime);

    glm::vec3 GetCelestialBodyDirection() const;
};
