#include "GetCanvasIntersectionPoint.h"


vec2 getCanvasIntersectionPoint(vec3 cameraPosition, vec3 vertexPosition, float focalLength, float scalingFactor, int width, int height, mat3 cameraOrientation) {
    vertexPosition = vertexPosition - cameraPosition;
    vertexPosition = cameraOrientation * vertexPosition + cameraPosition;

    float u = (-focalLength * (vertexPosition.x - cameraPosition.x) / (vertexPosition.z - cameraPosition.z)) * scalingFactor  + (width / 2);
    float v = (focalLength * (vertexPosition.y - cameraPosition.y) / (vertexPosition.z - cameraPosition.z )) * scalingFactor + (height / 2);
    return vec2(u, v);
}

