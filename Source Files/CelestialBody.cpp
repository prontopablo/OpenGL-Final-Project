#include "CelestialBody.h"
#include <cmath>

CelestialBody::CelestialBody(glm::vec3 startPos, float startAngle, float startRadius, float phaseDifference)
    : position(startPos), angle(startAngle + phaseDifference), radius(startRadius) {}

void CelestialBody::Update(float deltaTime) {
    angle += deltaTime * 0.05f;

    float targetX = 0.0f + radius * std::cos(0.25f * angle);
    float targetY = 100.0f + 200.0f * std::sin(0.25f * angle);

    position.x = targetX;
    position.y = targetY;
}

glm::vec3 CelestialBody::GetCelestialBodyDirection() const {
    return glm::normalize(position);
}
