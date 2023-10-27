#pragma once

#include <glm/glm.hpp>
using namespace glm;

vec2 getCanvasIntersectionPoint(vec3 cameraPosition, vec3 vertexPosition, float focalLength, float scalingFactor, int width, int height,mat3 cameraOrientation);
