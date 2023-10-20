#pragma once

#include <fstream>
#include <ModelTriangle.h>
#include <glm/glm.hpp>
#include <Utils.h>
using namespace std;
using namespace glm;

vector<ModelTriangle> loadObjFile(const std::string& filename, float scalingFactor);
void loadMtlFile(const std::string& filename,vector<pair<string, Colour>>& palette);