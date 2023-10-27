#pragma once
#include "DrawFile.h"
#include "Interpolate.h"
#include "LoadFile.h"
#include "cmath"
#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;



void orbit(vec3 &cameraPosition);
void Change_cameraPosition(vector<ModelTriangle> triangles, vec3 &cameraPosition, SDL_Event event, float focalLength, DrawingWindow &window);