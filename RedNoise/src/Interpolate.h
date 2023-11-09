#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "CanvasTriangle.h"
using namespace std;
using namespace glm;


vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
vector<vec3> interpolateThreeElementValues(vec3 from,vec3 to, int numberOfValues);
vector<CanvasPoint> interpolateTwoElementValues(CanvasPoint from,CanvasPoint to, int numberOfValues);
vector<TexturePoint> interpolateTwoElementValues(TexturePoint from, TexturePoint to, int numberOfValues);