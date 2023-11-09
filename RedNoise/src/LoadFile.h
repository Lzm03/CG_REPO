#pragma once

#include <fstream>
#include <ModelTriangle.h>
#include <glm/glm.hpp>
#include <Utils.h>
#include "DrawFile.h"

using namespace std;
using namespace glm;

vector<ModelTriangle> loadObjFile(const std::string& filename, float scalingFactor, unordered_map<string,Colour> colours);
unordered_map<string,Colour> loadMtlFile(const std::string& filename);