#include "CalculateDepth.h"


float calculateDepth(vec3 cameraPosition, vec3 vertexPosition) {
    float distance = vertexPosition.z - cameraPosition.z;
    float depth = 1.0f / distance;
    return depth;
}