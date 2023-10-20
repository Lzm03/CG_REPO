#pragma once
#include <DrawingWindow.h>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>
#include <ModelTriangle.h>
#include "CalculateDepth.h"

using namespace std;
using namespace glm;


// 函数声明
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
void drawTexturedTriangle(CanvasTriangle triangle, TextureMap texture, DrawingWindow &window);
void drawRenderTriangle(CanvasTriangle triangle, Colour input_colour, DrawingWindow &window,std::vector<std::vector<float>>& depthBuffer);
void drawPoint(vector<ModelTriangle> triangles,vec3 cameraPosition,float focalLength,DrawingWindow &window);
void drawWireframe(ModelTriangle triangle, vec3 cameraPosition, float focalLength, DrawingWindow &window);
void drawWireframeModel(vector<ModelTriangle> triangles, vec3 cameraPosition, float focalLength, DrawingWindow &window);
void drawTriangleModel(ModelTriangle triangle, vec3 cameraPosition, float focalLength, DrawingWindow &window, std::vector<std::vector<float>>& depthBuffer);
void drawRenderModel(vector<ModelTriangle> triangles, vec3 cameraPosition, float focalLength, DrawingWindow &window);