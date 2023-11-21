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
#include "GetCanvasIntersectionPoint.h"

using namespace std;
using namespace glm;

void draw(DrawingWindow &window) {
    window.clearPixels();
    // Week2 Task 3
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            float red = 0;
            float green = 0;
            float blue = 0;
            uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
            window.setPixelColour(x, y, colour);
        }
    }
}

void drawBlackAWhite(DrawingWindow &window) {
    window.clearPixels();
    // Week2 Task 3
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            float red = (1 - float(x) / float(window.width - 1)) * 255;
            float green = (1 - float(x) / float(window.width - 1)) * 255;
            float blue = (1 - float(x) / float(window.width - 1)) * 255;
            uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
            window.setPixelColour(x, y, colour);
        }
    }
}

// Week2 Task 5
void drawColour(DrawingWindow &window){
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow
    vec3 results;
    vector<vec3> Left = interpolateThreeElementValues(topLeft,bottomLeft,window.height);
    vector<vec3> Right = interpolateThreeElementValues(topRight,bottomRight,window.height);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            vec3 left = Left[y];
            vec3 right = Right[y];
            vector<vec3> row = interpolateThreeElementValues(left,right,window.width);
            results = row[x];
            uint32_t colour = (255 << 24) + (int(results.x) << 16) + (int(results.y) << 8) + int(results.z);
            window.setPixelColour(x, y, colour);
        }
    }
}

// Week3 Task 2
void drawline(CanvasPoint& from, CanvasPoint& to, Colour colour, DrawingWindow &window){
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff/numberOfSteps;
    float yStepSize = yDiff/numberOfSteps;
    for (float i=0.0; i<numberOfSteps; ++i) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        uint32_t Colour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
        window.setPixelColour(x, y, Colour);
    }
}

void drawLineFromTopLeftToCenter(DrawingWindow &window) {
    float x1 = 0;
    float y1 = 0;
    float x2 = window.width / 2;
    float y2 = window.height / 2;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

void drawLineFromTopRightToCenter(DrawingWindow &window) {
    float x1 = window.width - 1;
    float y1 = 0;
    float x2 = window.width / 2;
    float y2 = window.height / 2;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

void drawVerticalLine(DrawingWindow &window) {
    float x1 = window.width / 2;
    float y1 = 0;
    float x2 = window.width / 2;
    float y2 = window.height-1;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

void drawHorizontalLine(DrawingWindow &window) {
    float lineWidth = window.width / 3;
    float x1 = (window.width - lineWidth) / 2;
    float x2 = x1 + lineWidth;
    float y1 = window.height / 2;
    float y2 = y1;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

//void drawlineWithDepth(CanvasPoint& from, CanvasPoint& to, Colour colour, DrawingWindow &window, vector<vector<float>>& depthBuffer) {
//    float xDiff = to.x - from.x;
//    float yDiff = to.y - from.y;
//    float zDiff = to.depth - from.depth;
//    float numberOfSteps = 10 * std::max(abs(xDiff), abs(yDiff));
//    float xStepSize = xDiff/(numberOfSteps - 1);
//    float yStepSize = yDiff/(numberOfSteps - 1);
//    float zStepSize = zDiff/(numberOfSteps - 1);
//
//    for (float i=0.0; i<numberOfSteps - 1; ++i) {
//        float x = from.x + (xStepSize * i);
//        float y = from.y + (yStepSize * i);
//        float z = from.depth + (zStepSize * i);
//
//        int roundedX = round(x);
//        int roundedY = round(y);
//        if (roundedX >= 0 && roundedY >= 0 && roundedX < window.width && roundedY < window.height ){
//            if (z < depthBuffer[roundedX][roundedY]) {
//                uint32_t ColourVal = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
//                window.setPixelColour(round(x), round(y), ColourVal);
//                depthBuffer[roundedX][roundedY] = z;
//            }
//        }
//    }
//}

// Week3 Task 3
void drawTriangle(CanvasTriangle triangle,Colour input_colour, DrawingWindow &window){
    drawline(triangle.v0(),triangle.v1(),input_colour,window);
    drawline(triangle.v1(),triangle.v2(),input_colour,window);
    drawline(triangle.v2(),triangle.v0(),input_colour,window);
}

// Week3 Task 4 - second version
void drawFilledTriangle(CanvasTriangle triangle, Colour input_colour, Colour line_Colour,DrawingWindow &window) {
    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());
    CanvasPoint top = triangle.v0();
    CanvasPoint middle = triangle.v1();
    CanvasPoint bottom = triangle.v2();
    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
    float left_x = top.x;
    float right_x = top.x;
    // first part of triangle
    for (float y = top.y; y <= middle.y; ++y) {
        // start from top point
        int startX = round(right_x);
        int endX = round(left_x);
        CanvasPoint from(startX,y);
        CanvasPoint to(endX,y);
        drawline(from,to,input_colour,window);
        right_x += slope1;
        left_x += slope2;
    }
    // The slope of the third side
    right_x = middle.x;
    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);
    // second part of triangle
    for (float y = middle.y; y <= bottom.y; y++) {
        // start from middle point
        int startX = round(right_x);
        int endX = round(left_x);
        CanvasPoint from(startX,y);
        CanvasPoint to(endX,y);
        drawline(from,to,input_colour,window);
        right_x += slope3;
        left_x += slope2;
    }
    // draw three white line of whole triangle
    drawTriangle(triangle,line_Colour,window);
}

vec3 calculateFaceNormal(ModelTriangle &triangle){
    vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
    vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
    vec3 normal = normalize(cross(edge1, edge2));
    return normal;
}

float getProximityLighting(vec3 &light,vec3 &point){
    float distance = length(light - point); //distanceFromLightToPoint
    float Intensity =  40 /(4*M_PI*(pow(distance,2))); // intensity at surface of sphere
    float proximityLighting = std::max(0.0f, std::min(1.0f,Intensity)); // Range of brightness;
    return proximityLighting;
}

float getIncidentLight(vec3 &light,vec3 &point, vec3 &faceNormal){
    vec3 lightDirection = normalize(light - point);
    float angleOfIncidence = std::max(0.0f,std::min(1.0f,dot(normalize(faceNormal), lightDirection))); // Range of brightness
    return angleOfIncidence;
}

float getReflectionLight(vec3 &light, vec3 &cameraPosition, vec3 &point, vec3 &normal, float specularExponent) {
    vec3 lightDirection = normalize(light - point);
    vec3 cameraDirection = normalize(cameraPosition - point);
    vec3 reflectDirection = normalize(normalize(-lightDirection) - (normal * 2.0f * dot(normalize(-lightDirection), normal)));
    float reflectionLight = pow(dot(cameraDirection, reflectDirection), specularExponent);
    return reflectionLight;
}

bool is_shadow(RayTriangleIntersection intersection, vector<ModelTriangle> &triangles,vec3 &light) {
    vec3 shadow_rayDirection = light - intersection.intersectionPoint;
    for(int i = 0; i < triangles.size(); i++) {
        ModelTriangle triangle = triangles[i];
        vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        vec3 SPVector = intersection.intersectionPoint - triangle.vertices[0];
        mat3 DEMatrix(-normalize(shadow_rayDirection), e0, e1);
        vec3 possibleSolution = inverse(DEMatrix) * SPVector;
        float t = possibleSolution.x, u = possibleSolution.y, v = possibleSolution.z;

        if((u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0) {
            if(t < length(shadow_rayDirection) && t > 0.01 && i != intersection.triangleIndex) {
                return true;
            }
        }
    }
    return false;
}

RayTriangleIntersection getClosestValidIntersection(vector<ModelTriangle> &triangles, vec3 cameraPosition, vec3 rayDirection) {
    RayTriangleIntersection r;
    r.distanceFromCamera = numeric_limits<float>::infinity();
    for (int i = 0; i < triangles.size(); ++i) {
        ModelTriangle triangle = triangles[i];
        vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        vec3 SPVector = cameraPosition - triangle.vertices[0];
        mat3 DEMatrix(-rayDirection, e0, e1);
        vec3 possibleSolution = inverse(DEMatrix) * SPVector;
        float t = possibleSolution.x;
        float u = possibleSolution.y;
        float v = possibleSolution.z;
        if (t > 0 && u >= 0 && u <= 1.0 && v >= 0.0 && v <= 1.0 && (u + v) <= 1.0) {
            if (r.distanceFromCamera > t) {
                vec3 intersectionPoint = cameraPosition + possibleSolution.x * rayDirection;
                float distanceFromCamera = t;
                r = RayTriangleIntersection(intersectionPoint, distanceFromCamera, triangle, i);
            }
        }
    }
    return r;
}

vec3 getRayDirectionFromCanvas(int x, int y,int width, int height, float focalLength, float scalingFactor, mat3 cameraOrientation, vec3 &camerPosition) {
    vec3 rayDirectionWorldSpace;
    float ndcX = (x - camerPosition.x - (width / 2.0f)) / (focalLength * scalingFactor);
    float ndcY = (y - camerPosition.y - (height / 2.0f)) / (focalLength * scalingFactor);
    vec3 rayDirectionCameraSpace(-ndcX , -ndcY, -focalLength);
    rayDirectionWorldSpace = normalize(cameraOrientation * rayDirectionCameraSpace);
    return rayDirectionWorldSpace;
}

void drawRayTracedScene(vector<ModelTriangle> &triangles, DrawingWindow &window, vec3 &cameraPosition, float focalLength, mat3 &cameraOrientation,vec3 &light,float scaling_factor) {
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x,y,window.width,window.height,focalLength,scaling_factor,cameraOrientation,cameraPosition);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, cameraPosition, rayDirection);
            if (!isinf(r.distanceFromCamera)) {
                r.intersectedTriangle.normal = calculateFaceNormal(r.intersectedTriangle);
                float Proximity = getProximityLighting(light,r.intersectionPoint);
                float AngleOfIncidence = getIncidentLight(light,r.intersectionPoint,r.intersectedTriangle.normal);
                float specularExponent = 256.0f;
                float reflectionLight = getReflectionLight(light,cameraPosition,r.intersectionPoint,r.intersectedTriangle.normal,specularExponent);

                Colour colour = r.intersectedTriangle.colour;

                // Diffuse lighting(with Ambient lighting)
                float Diffuselight = glm::clamp(Proximity * AngleOfIncidence,0.5f,1.0f);
                colour.red = colour.red * Diffuselight;
                colour.green = colour.green * Diffuselight;
                colour.blue = colour.blue * Diffuselight;

                //Specular highlighting
                colour.red = std::min((colour.red + reflectionLight*255), 255.0f);
                colour.green = std::min((colour.green + reflectionLight*255), 255.0f);
                colour.blue = std::min((colour.blue + reflectionLight*255), 255.0f);

                uint32_t ray_Color = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                if (!is_shadow(r, triangles, light)) {
                    window.setPixelColour(x, y, ray_Color);
                }
                else{
                    uint32_t shadowColor = (255 << 24) + (colour.red/2 << 16) + (colour.green/2 << 8) + colour.blue/2;
                    window.setPixelColour(x, y, shadowColor);
                }
            }
        }
    }
}

vector<ModelTriangle> calculateVertexNormals(vector<ModelTriangle> &triangles) {
    for (auto& triangle : triangles) {
        triangle.normal = calculateFaceNormal(triangle);
    }
    for (auto& triangle : triangles) {
        triangle.vertexNormals.resize(triangle.vertices.size());
        for (int v = 0; v < triangle.vertices.size(); ++v) {
            vec3 vertexNormal = vec3(0.0f);
            int count = 0;
            for (auto& otherTriangle : triangles) {
                for (auto& otherVertex : otherTriangle.vertices) {
                    if (triangle.vertices[v] == otherVertex) {
                        vertexNormal += otherTriangle.normal;
                        count++;
                    }
                }
            }
            if (count != 0) {
                triangle.vertexNormals[v] = normalize(vertexNormal / static_cast<float>(count));
            }
        }
    }
    return triangles;
}

Colour calculateLightingAtVertex(RayTriangleIntersection r, vec3 &normal, vec3 &light, vec3 &cameraPosition, float shininess ) {
    float ambientStrength = 0.2f;
    Colour ambientColour = {
            int(r.intersectedTriangle.colour.red * ambientStrength),
            int(r.intersectedTriangle.colour.green * ambientStrength),
            int(r.intersectedTriangle.colour.blue * ambientStrength)
    };

    vec3 toLight = light - r.intersectionPoint;
    float distance = length(toLight);
    float Intensity =  100 /(4*M_PI*(pow(distance,2)));
    float attenuation = std::max(0.0f, std::min(1.0f,Intensity));

    vec3 lightDirection = normalize(toLight);
    float diff = std::max(dot(normal, lightDirection), 0.0f);
    Colour diffuseColour = {
            int(r.intersectedTriangle.colour.red * diff * attenuation),
            int(r.intersectedTriangle.colour.green * diff * attenuation),
            int(r.intersectedTriangle.colour.blue * diff * attenuation)
    };

    vec3 viewDir = normalize(cameraPosition - r.intersectionPoint);
    vec3 reflectDir = normalize(normalize(-lightDirection) - (normal * 2.0f * dot(normalize(-lightDirection), normal)));
    float spec = pow(std::max(dot(viewDir, reflectDir), 0.0f), shininess);
    Colour specularColour = {
            int(255.0f * spec),
            int(255.0f *  spec),
            int(255.0f * spec)
    };

    int finalRed = std::min(ambientColour.red + diffuseColour.red + specularColour.red, 255);
    int finalGreen = std::min(ambientColour.green + diffuseColour.green + specularColour.green, 255);
    int finalBlue = std::min(ambientColour.blue + diffuseColour.blue + specularColour.blue, 255);

    return Colour{finalRed, finalGreen, finalBlue};
}


Colour interpolateVertexColours(ModelTriangle &triangle, vec3 &barycentric) {
    Colour colour;
    colour.red = barycentric.x * triangle.vertexColours[0].red + barycentric.y * triangle.vertexColours[1].red + barycentric.z * triangle.vertexColours[2].red;
    colour.green = barycentric.x * triangle.vertexColours[0].green + barycentric.y * triangle.vertexColours[1].green + barycentric.z * triangle.vertexColours[2].green;
    colour.blue = barycentric.x * triangle.vertexColours[0].blue + barycentric.y * triangle.vertexColours[1].blue + barycentric.z * triangle.vertexColours[2].blue;
    return colour;
}

uint32_t convertColourToUint(Colour &colour) {
    uint32_t r = static_cast<uint32_t>(std::min(std::max(colour.red, 0), 255));
    uint32_t g = static_cast<uint32_t>(std::min(std::max(colour.green, 0), 255));
    uint32_t b = static_cast<uint32_t>(std::min(std::max(colour.blue, 0), 255));
    return (255 << 24) + (r << 16) + (g << 8) + b;
}

vec3 getBarycentricCoordinates(vec3 &point, ModelTriangle &triangle) {
    vec3 v0 = triangle.vertices[1] - triangle.vertices[0];
    vec3 v1 = triangle.vertices[2] - triangle.vertices[0];
    vec3 v2 = point - triangle.vertices[0];

    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return vec3(u, v, w);
}

void drawPhong(vector<ModelTriangle> triangles, DrawingWindow &window, vec3 &cameraPosition, float focalLength, mat3 &cameraOrientation,vec3 &light,float scaling_factor){
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x,y,window.width,window.height,focalLength,scaling_factor,cameraOrientation,cameraPosition);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, cameraPosition, rayDirection);
            for (auto& triangle : triangles) {
                triangle.vertexColours.resize(triangle.vertices.size());
                for (int v = 0; v < triangle.vertices.size(); ++v) {
                    triangle.vertexColours[v] = calculateLightingAtVertex(r,triangle.vertexNormals[v],light,cameraPosition,64.0f);
                }
            }

            if (!isinf(r.distanceFromCamera)) {
                vec3 barycentric = getBarycentricCoordinates(r.intersectionPoint, r.intersectedTriangle);
                Colour interpolatedColour = interpolateVertexColours(r.intersectedTriangle, barycentric);
                if (is_shadow(r, triangles, light)) {
                    interpolatedColour = Colour{int(interpolatedColour.red), int(interpolatedColour.green), int(interpolatedColour.blue)};
                }
                window.setPixelColour(x, y, convertColourToUint(interpolatedColour));
            }
        }
    }
}


void drawGouraud(vector<ModelTriangle> triangles, DrawingWindow &window, vec3 &cameraPosition, float focalLength, mat3 &cameraOrientation,vec3 &light,float scaling_factor) {
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x,y,window.width,window.height,focalLength,scaling_factor,cameraOrientation,cameraPosition);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, cameraPosition, rayDirection);

            for (auto& triangle : triangles) {
                for (int v = 0; v < triangle.vertices.size(); ++v) {
                    float Proximity = getProximityLighting(light, r.intersectionPoint);
                    float AngleOfIncidence = getIncidentLight(light, r.intersectionPoint, triangle.vertexNormals[v]);
                    float specularExponent = 256.0f;
                    float reflectionLight = getReflectionLight(light, cameraPosition, r.intersectionPoint,
                                                               triangle.vertexNormals[v], specularExponent);
                    if (!isinf(r.distanceFromCamera)) {

                        Colour colour = r.intersectedTriangle.colour;
                        // Diffuse lighting(with Ambient lighting)
                        float Diffuselight = glm::clamp(Proximity * AngleOfIncidence, 0.5f, 1.0f);
                        colour.red = colour.red * Diffuselight;
                        colour.green = colour.green * Diffuselight;
                        colour.blue = colour.blue * Diffuselight;

                        //Specular highlighting
                        colour.red = std::min((colour.red + reflectionLight * 255), 255.0f);
                        colour.green = std::min((colour.green + reflectionLight * 255), 255.0f);
                        colour.blue = std::min((colour.blue + reflectionLight * 255), 255.0f);

                        uint32_t ray_Color =
                                (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                        if (!is_shadow(r, triangles, light)) {
                            window.setPixelColour(x, y, ray_Color);
                        } else {
                            uint32_t shadowColor =
                                    (255 << 24) + (colour.red << 16) + (colour.green  << 8) + colour.blue / 2;
                            window.setPixelColour(x, y, shadowColor);
                        }
                    }
                }
            }
        }
    }
}



// Week3 Task 5
//void drawTexturedTriangle(CanvasTriangle triangle, TextureMap texture, DrawingWindow &window,vector<std::vector<float>>& depthBuffer) {
//    // Sort vertices by y-coordinate
//    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
//    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
//    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());
//
//    CanvasPoint top = triangle.v0();
//    CanvasPoint middle = triangle.v1();
//    CanvasPoint bottom = triangle.v2();
//
//    // Calculate slopes for the top and bottom edges
//    float slope1 = (middle.x - top.x) / (middle.y - top.y);
//    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
//    float depth_slope1 = (middle.depth - top.depth) / (middle.y - top.y);
//    float depth_slope2 = (bottom.depth - top.depth) / (bottom.y - top.y);
//
//    float left_x = top.x;
//    float right_x = top.x;
//    float left_depth = top.depth;
//    float right_depth = top.depth;
//
//    // First part of triangle (top to middle)
//    for (int y = top.y; y <= middle.y; ++y) {
//        // Calculate the corresponding texturePoints for CanvasPoints
//        float t1 = (y - top.y) / (middle.y - top.y);
//        float t2 = (y - top.y) / (bottom.y - top.y);
//        float texX1 = top.texturePoint.x + (middle.texturePoint.x - top.texturePoint.x) * t1;
//        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
//        float texY1 = top.texturePoint.y + (middle.texturePoint.y - top.texturePoint.y) * t1;
//        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;
//
//        // Draw the horizontal line with texture mapping
//        for (int x = round(left_x); x <= round(right_x); ++x) {
//            // Interpolate t based on the current x position
//            float t = (x - left_x) / (right_x - left_x);
//            int texX = static_cast<int>(texX1 + (texX2 - texX1) * t);
//            int texY = static_cast<int>(texY1 + (texY2 - texY1) * t);
//
//            // Calculate the index to access the texture pixel
//            if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
//                int index = texY * texture.width + texX;
//                window.setPixelColour(x, y, texture.pixels[index]);
//            }
//        }
//        left_x += slope1;
//        right_x += slope2;
//    }
//
//    // Initialize left and right x-coordinates for the bottom edge
//    left_x = middle.x;
//    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);
//
//    // Second part of triangle (middle to bottom)
//    for (int y = middle.y + 1; y <= bottom.y; ++y) {
//        // Calculate the corresponding texture coordinates for Canvas Points
//        float t1 = (y - middle.y) / (bottom.y - middle.y);
//        float t2 = (y - top.y) / (bottom.y - top.y);
//        float texX1 = middle.texturePoint.x + (bottom.texturePoint.x - middle.texturePoint.x) * t1;
//        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
//        float texY1 = middle.texturePoint.y + (bottom.texturePoint.y - middle.texturePoint.y) * t1;
//        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;
//
//        // Draw the horizontal line with texture mapping
//        for (int x = round(left_x); x <= round(right_x); ++x) {
//            // Interpolate t based on the current x position
//            float t = (x - left_x) / (right_x - left_x);
//            int texX = static_cast<int>(texX1 + (texX2 - texX1) * t);
//            int texY = static_cast<int>(texY1 + (texY2 - texY1) * t);
//
//            // Calculate the index to access the texture pixel
//
//            if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
//                int index = texY * texture.width + texX;
//                window.setPixelColour(x, y, texture.pixels[index]);
//            }
//        }
//        left_x += slope3;
//        right_x += slope2;
//    }
//}

void drawTexturedTriangle(CanvasTriangle triangle, TextureMap texture, DrawingWindow &window, vector<vector<float>>& depthBuffer) {
    // Sort vertices by y-coordinate
    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());

    CanvasPoint top = triangle.v0();
    CanvasPoint middle = triangle.v1();
    CanvasPoint bottom = triangle.v2();

    // Calculate slopes and depth slopes for the edges
    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
    float depth_slope1 = (middle.depth - top.depth) / (middle.y - top.y);
    float depth_slope2 = (bottom.depth - top.depth) / (bottom.y - top.y);

    float left_x = top.x;
    float right_x = top.x;
    float left_depth = top.depth;
    float right_depth = top.depth;

    // Draw the top to middle part of the triangle
    for (int y = top.y; y <= middle.y; ++y) {
        float t1 = (y - top.y) / (middle.y - top.y);
        float t2 = (y - top.y) / (bottom.y - top.y);
        float texX1 = top.texturePoint.x + (middle.texturePoint.x - top.texturePoint.x) * t1;
        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
        float texY1 = top.texturePoint.y + (middle.texturePoint.y - top.texturePoint.y) * t1;
        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;

        // Draw horizontal line with texture and depth buffer checking
        for (int x = round(left_x); x <= round(right_x); ++x) {
            float t = (x - left_x) / (right_x - left_x);
            float current_depth = left_depth + (right_depth - left_depth) * t;
            if (x >= 0 && x < window.width && y >= 0 && y < window.height && current_depth < depthBuffer[y][x]) {
                int texX = static_cast<int>(texX1 + (texX2 - texX1) * t);
                int texY = static_cast<int>(texY1 + (texY2 - texY1) * t);

                if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
                    int index = texY * texture.width + texX;
                    window.setPixelColour(x, y, texture.pixels[index]);
                    depthBuffer[y][x] = current_depth;
                }
            }
        }
        left_x += slope1;
        right_x += slope2;
        left_depth += depth_slope1;
        right_depth += depth_slope2;
    }

    // Reinitialize slopes for the bottom part of the triangle
    left_x = middle.x;
    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);
    float depth_slope3 = (bottom.depth - middle.depth) / (bottom.y - middle.y);
    left_depth = middle.depth;

    // Draw the middle to bottom part of the triangle
    for (int y = middle.y + 1; y <= bottom.y; ++y) {
        float t1 = (y - middle.y) / (bottom.y - middle.y);
        float t2 = (y - top.y) / (bottom.y - top.y);
        float texX1 = middle.texturePoint.x + (bottom.texturePoint.x - middle.texturePoint.x) * t1;
        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
        float texY1 = middle.texturePoint.y + (bottom.texturePoint.y - middle.texturePoint.y) * t1;
        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;
        for (int x = round(left_x); x <= round(right_x); ++x) {
            float t = (x - left_x) / (right_x - left_x);
            float current_depth = left_depth + (right_depth - left_depth) * t;
            if (x >= 0 && x < window.width && y >= 0 && y < window.height && current_depth < depthBuffer[y][x]) {
                int texX = static_cast<int>(texX1 + (texX2 - texX1) * t);
                int texY = static_cast<int>(texY1 + (texY2 - texY1) * t);

                if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
                    int index = texY * texture.width + texX;
                    window.setPixelColour(x, y, texture.pixels[index]);
                    depthBuffer[y][x] = current_depth;
                }
            }
        }
        left_x += slope3;
        right_x += slope2;
        left_depth += depth_slope3;
        right_depth += depth_slope2;

    }
}


//void drawRenderTriangle(CanvasTriangle triangle, Colour input_colour, DrawingWindow &window,vector<std::vector<float>>& depthBuffer) {
//    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
//    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
//    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());
//    CanvasPoint top = triangle.v0();
//    CanvasPoint middle = triangle.v1();
//    CanvasPoint bottom = triangle.v2();
//    float slope1 = (middle.x - top.x) / (middle.y - top.y);
//    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
//    float depth_slope1 = (middle.depth - top.depth) / (middle.y - top.y);
//    float depth_slope2 = (bottom.depth - top.depth) / (bottom.y - top.y);
//    float left_x = top.x;
//    float right_x = top.x;
//    float left_depth = top.depth;
//    float right_depth = top.depth;
//
//    for (float y = top.y; y < middle.y; ++y) {
//        CanvasPoint from(round(left_x), round(y), left_depth);
//        CanvasPoint to(round(right_x), round(y), right_depth);
//        drawlineWithDepth(from, to, input_colour, window, depthBuffer);
//
//        right_x += slope1;
//        left_x += slope2;
//
//        left_depth += depth_slope2;
//        right_depth += depth_slope1;
//    }
//
//    right_x = middle.x;
//    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);
//    float depth_slope3 = (bottom.depth - middle.depth) / (bottom.y - middle.y);
//
//
//    for (float y = middle.y; y <= bottom.y; y++) {
//        CanvasPoint from(round(left_x), round(y), left_depth);
//        CanvasPoint to(round(right_x), round(y), right_depth);
//        drawlineWithDepth(from, to, input_colour, window, depthBuffer);
//        right_x += slope3;
//        left_x += slope2;
//
//        left_depth += depth_slope2;
//        right_depth += depth_slope3;
//    }
//}

//void drawPoint(vector<ModelTriangle> triangles,vec3 cameraPosition,float focalLength,DrawingWindow &window){
//    for (int i = 0; i < triangles.size(); ++i) {
//        for (int j = 0; j < 3; ++j) {
//            vec3 vertexPosition = triangles[i].vertices[j];
//            float scaling_factor = 186;
//            vec2 canvasPoint = getCanvasIntersectionPoint(cameraPosition, vertexPosition,scaling_factor,focalLength,window.width,window.height);
//            cout << "("<< canvasPoint.x << "," << canvasPoint.y << ")" << endl;
//            // Flip the Y-axis
//            Colour colour = Colour{255, 255, 255};
//            uint32_t input_Colour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
//            window.setPixelColour(canvasPoint.x, canvasPoint.y, input_Colour);
//        }
//    }
//}

void drawWireframe(vector<ModelTriangle> &triangles, vec3 cameraPosition, float focalLength, DrawingWindow &window, mat3 &cameraOrientation,float scaling_factor) {
    CanvasPoint p1;
    CanvasPoint p2;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < triangles.size(); ++j) {
            ModelTriangle triangle = triangles[j];
            vec3 vertexPosition1 = triangle.vertices[i];
            vec3 vertexPosition2 = triangle.vertices[(i + 1) %3];
            vec2 canvasPoint1 = getCanvasIntersectionPoint(cameraPosition, vertexPosition1, focalLength, scaling_factor,
                                                           window.width, window.height, cameraOrientation);
            vec2 canvasPoint2 = getCanvasIntersectionPoint(cameraPosition, vertexPosition2, focalLength, scaling_factor,
                                                           window.width, window.height, cameraOrientation);
            p1 = CanvasPoint{round(canvasPoint1.x), round(canvasPoint1.y)};
            p2 = CanvasPoint{round(canvasPoint2.x), round(canvasPoint2.y)};
            drawline(p1, p2, triangle.colour, window);
        }
    }
}

void drawRenderTriangle(CanvasTriangle triangle, Colour input_colour, DrawingWindow &window, vector<vector<float>>& depthBuffer) {
    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());

    CanvasPoint top = triangle.v0();
    CanvasPoint middle = triangle.v1();
    CanvasPoint bottom = triangle.v2();

    int xMin = std::min({top.x, middle.x, bottom.x});
    int xMax = std::max({top.x, middle.x, bottom.x});
    int yMin = std::min({top.y, middle.y, bottom.y});
    int yMax = std::max({top.y, middle.y, bottom.y});

    float triangleArea = (top.x * (middle.y - bottom.y) + middle.x * (bottom.y - top.y) + bottom.x * (top.y - middle.y));
    for (int y = yMin; y <= yMax; y++) {
        for (int x = xMin; x <= xMax; x++) {
            float lambda1 = ((x * (middle.y - bottom.y) + middle.x * (bottom.y - y) + bottom.x * (y - middle.y)) / triangleArea);
            float lambda2 = ((top.x * (y - bottom.y) + x * (bottom.y - top.y) + bottom.x * (top.y - y)) / triangleArea);
            float lambda3 = 1.0f - lambda1 - lambda2;

            if (lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0) {
                float depth = top.depth * lambda1 + middle.depth * lambda2 + bottom.depth * lambda3;
                if (x >= 0 && y >= 0 && x < window.width && y < window.height && depth < depthBuffer[x][y]) {
                    uint32_t colourVal = (255 << 24) + (input_colour.red << 16) + (input_colour.green << 8) + input_colour.blue;
                    window.setPixelColour(x, y, colourVal);
                    depthBuffer[x][y] = depth;
                }
            }
        }
    }
}

void drawRasterised(ModelTriangle triangle, vec3 cameraPosition, float focalLength, DrawingWindow &window, vector<vector<float>>& depthBuffer, mat3 cameraOrientation,float scaling_factor) {
    CanvasTriangle canvasTriangle;
    RayTriangleIntersection r;
    for (int i = 0; i < 3; ++i) {
        vec3 vertexPosition = triangle.vertices[i];
        vec2 canvasPoint = getCanvasIntersectionPoint(cameraPosition, vertexPosition, focalLength, scaling_factor, window.width, window.height, cameraOrientation);
        vec3 cameraSpaceVertex = cameraOrientation * (vertexPosition - cameraPosition);
        canvasTriangle.vertices[i] = CanvasPoint(canvasPoint.x, canvasPoint.y, -cameraSpaceVertex.z);
        canvasTriangle.vertices[i].texturePoint = triangle.texturePoints[i];
    }

    Colour inputColour = triangle.colour;
    if (triangle.colour.name != ""){
        TextureMap texture = TextureMap("../" + triangle.colour.name);
        for(int i = 0; i < canvasTriangle.vertices.size(); i++) {
            canvasTriangle.vertices[i].texturePoint.x *= texture.width;
            canvasTriangle.vertices[i].texturePoint.y *= texture.height;
        }
        drawTexturedTriangle(canvasTriangle,texture,window,depthBuffer);
    }
    else {
        drawRenderTriangle(canvasTriangle, inputColour, window, depthBuffer);
    }
}

void drawRasterisedModel(vector<ModelTriangle> &triangles, vec3 &cameraPosition, float focalLength, DrawingWindow &window, mat3 &cameraOrientation,float scaling_factor) {
    int width = window.width;
    int height = window.height;
    vector<vector<float>> depthBuffer(width, vector<float>(height, numeric_limits<float>::infinity()));
    for (int i = 0; i < triangles.size(); ++i) {
        drawRasterised(triangles[i], cameraPosition, focalLength, window, depthBuffer, cameraOrientation,scaling_factor);
    }
}

