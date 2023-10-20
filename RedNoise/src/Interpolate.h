#pragma once
#include <vector>
#include <glm/glm.hpp>
using namespace std;
using namespace glm;


vector<float> interpolateSingleFloats(float from, float to, int numberOfValues);
vector<vec3> interpolateThreeElementValues(vec3 from,vec3 to, int numberOfValues);