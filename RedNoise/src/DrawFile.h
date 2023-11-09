#pragma once
#include <DrawingWindow.h>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include "Interpolate.h"
#include "RayTriangleIntersection.h"

using namespace std;
using namespace glm;


void draw(DrawingWindow &window);
void drawColour(DrawingWindow &window);
void drawColour(DrawingWindow &window);
void drawline(CanvasPoint& from, CanvasPoint& to, Colour colour, DrawingWindow &window);
void drawLineFromTopLeftToCenter(DrawingWindow &window);
void drawLineFromTopRightToCenter(DrawingWindow &window);
void drawVerticalLine(DrawingWindow &window);
void drawHorizontalLine(DrawingWindow &window);
void drawlineWithDepth(CanvasPoint& from, CanvasPoint& to, Colour colour, DrawingWindow &window, std::vector<std::vector<float>>& depthBuffer);
void drawTriangle(CanvasTriangle triangle,Colour input_colour, DrawingWindow &window);
void drawFilledTriangle(CanvasTriangle triangle, Colour input_colour, Colour line_Colour,DrawingWindow &window);
void drawPoint(vector<ModelTriangle> triangles,vec3 cameraPosition,float focalLength,DrawingWindow &window);
void drawWireframe(vector<ModelTriangle> &triangles, vec3 &cameraPosition, float focalLength, DrawingWindow &window, mat3 &cameraOrientation);
void drawRasterised(ModelTriangle triangle, vec3 cameraPosition, float focalLength, DrawingWindow &window, std::vector<std::vector<float>>& depthBuffer, mat3 cameraOrientation);
void drawRasterisedModel(vector<ModelTriangle> &triangles, vec3 &cameraPosition, float focalLength, DrawingWindow &window, mat3 &cameraOrientation);
RayTriangleIntersection getClosestValidIntersection(vector<ModelTriangle> triangles,vec3 cameraPosition, vec3 rayDirection);
void drawRayTracedScene(vector<ModelTriangle> &triangles, DrawingWindow &window, vec3 &cameraPosition, float focalLength, mat3 &cameraOrientation);

