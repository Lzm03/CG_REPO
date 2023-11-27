#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include "CanvasPoint.h"
#include "Colour.h"
#include "ModelTriangle.h"
#include "RayTriangleIntersection.h"
#include "TextureMap.h"
#include "TexturePoint.h"
#include <unordered_map>
#include <glm/glm.hpp>


using namespace std;
using namespace glm;

#define WIDTH 320 * 4
#define HEIGHT 240 * 4

bool orbiting = false;
float refractiveIndexRatio = 1.5f;
mat3 cameraOrientation(
        vec3(-1.0,0.0,0.0),
        vec3(0.0, 1.0,0.0),
        vec3(0.0,0.0,1.0)
);
vec3 cameraPosition(0.0f, 0.0f, 4.0f);
vec3 SphereLight(0.0f,1.5f, 3.5f);
vec3 BoxLight(0.0,0.5,0.2);
vector<vec3> circleLights;

Colour processRayIntersection(RayTriangleIntersection r, vec3 rayDirection, vector<ModelTriangle> triangles, vector<vec3> Mult_lights, int depth);

enum RenderMode {
    RENDER_NONE,
    RENDER_WIREFRAME,
    RENDER_RASTERISED,
    RENDER_RayTRACED,
    RENDER_FlatSphere,
    RENDER_Texture,
    RENDER_GOURAUD,
    RENDER_PHONG,
    RENDER_LOGO
};
RenderMode currentRenderMode = RENDER_NONE;

//void draw(DrawingWindow &window) {
//    window.clearPixels();
//    for (size_t y = 0; y < window.height; y++) {
//        for (size_t x = 0; x < window.width; x++) {
//            float red = 0;
//            float green = 0;
//            float blue = 0;
//            uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
//            window.setPixelColour(x, y, colour);
//        }
//    }
//}

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

vec2 getCanvasIntersectionPoint(vec3 vertexPosition, float focalLength, float scalingFactor, int width, int height) {
    vertexPosition = vertexPosition - cameraPosition;
    vertexPosition = cameraOrientation * vertexPosition + cameraPosition;

    float u = (focalLength * (vertexPosition.x - cameraPosition.x) / (vertexPosition.z - cameraPosition.z)) * scalingFactor  + (width / 2);
    float v = (focalLength * (vertexPosition.y - cameraPosition.y) / (vertexPosition.z - cameraPosition.z )) * scalingFactor + (height / 2);
    return vec2(u, v);
}

void renderWireframe(vector<ModelTriangle> triangles, DrawingWindow &window,float focalLength, float scaling_factor) {
    CanvasPoint p1;
    CanvasPoint p2;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < triangles.size(); ++j) {
            ModelTriangle triangle = triangles[j];
            vec3 vertexPosition1 = triangle.vertices[i];
            vec3 vertexPosition2 = triangle.vertices[(i + 1) %3];
            vec2 canvasPoint1 = getCanvasIntersectionPoint(vertexPosition1, focalLength, scaling_factor,
                                                           window.width, window.height);
            vec2 canvasPoint2 = getCanvasIntersectionPoint(vertexPosition2, focalLength, scaling_factor,
                                                           window.width, window.height);
            p1 = CanvasPoint{round(canvasPoint1.x), round(canvasPoint1.y)};
            p2 = CanvasPoint{round(canvasPoint2.x), round(canvasPoint2.y)};
            drawline(p1, p2, triangle.colour, window);
        }
    }
}

void Texturing(ModelTriangle triangle, TextureMap texture, DrawingWindow &window, vector<vector<float>>& depthBuffer) {
    if (triangle.vertices[0].y > triangle.vertices[1].y) swap(triangle.vertices[0], triangle.vertices[1]);
    if (triangle.vertices[0].y > triangle.vertices[2].y) swap(triangle.vertices[0], triangle.vertices[2]);
    if (triangle.vertices[1].y > triangle.vertices[2].y) swap(triangle.vertices[1], triangle.vertices[2]);

    vec3 top = vec3(triangle.vertices[0].x,triangle.vertices[0].y,triangle.vertices[0].z);
    vec3 middle = vec3(triangle.vertices[1].x,triangle.vertices[1].y,triangle.vertices[1].z);
    vec3 bottom = vec3(triangle.vertices[2].x,triangle.vertices[2].y,triangle.vertices[2].z);

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
                float depth = top.z * lambda1 + middle.z * lambda2 + bottom.z * lambda3;
                if (x >= 0 && y >= 0 && x < window.width && y < window.height && depth < depthBuffer[x][y]) {
                    float texX = triangle.texturePoints[0].x * lambda1 + triangle.texturePoints[1].x * lambda2 + triangle.texturePoints[2].x * lambda3;
                    float texY = triangle.texturePoints[0].y * lambda1 + triangle.texturePoints[1].y * lambda2 + triangle.texturePoints[2].y * lambda3;

                    if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
                        uint32_t index = round(texY) * texture.width + round(texX);
                        window.setPixelColour(x, y, texture.pixels[index]);
                        depthBuffer[x][y] = depth;
                    }
                }
            }
        }
    }
}

void rasterCalculate(ModelTriangle triangle, Colour input_colour, DrawingWindow &window, vector<vector<float>>& depthBuffer) {
    if (triangle.vertices[0].y > triangle.vertices[1].y) swap(triangle.vertices[0], triangle.vertices[1]);
    if (triangle.vertices[0].y > triangle.vertices[2].y) swap(triangle.vertices[0], triangle.vertices[2]);
    if (triangle.vertices[1].y > triangle.vertices[2].y) swap(triangle.vertices[1], triangle.vertices[2]);

    vec3 top = vec3(triangle.vertices[0].x,triangle.vertices[0].y,triangle.vertices[0].z);
    vec3 middle = vec3(triangle.vertices[1].x,triangle.vertices[1].y,triangle.vertices[1].z);
    vec3 bottom = vec3(triangle.vertices[2].x,triangle.vertices[2].y,triangle.vertices[2].z);

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
                float depth = top.z * lambda1 + middle.z * lambda2 + bottom.z * lambda3;
                if (x >= 0 && y >= 0 && x < window.width && y < window.height && depth < depthBuffer[x][y]) {
                    uint32_t colourVal = (255 << 24) + (input_colour.red << 16) + (input_colour.green << 8) + input_colour.blue;
                    window.setPixelColour(x, y, colourVal);
                    depthBuffer[x][y] = depth;
                }
            }
        }
    }
}

void Rasterising(ModelTriangle triangle, float focalLength, DrawingWindow &window, vector<vector<float>>& depthBuffer,float scaling_factor) {
    for (int i = 0; i < 3; ++i) {
        vec2 canvasPoint = getCanvasIntersectionPoint(triangle.vertices[i], focalLength, scaling_factor, window.width,window.height);
        vec3 cameraSpaceVertex = cameraOrientation * (triangle.vertices[i] - cameraPosition);
        triangle.vertices[i] = vec3(canvasPoint.x, canvasPoint.y, -cameraSpaceVertex.z);
    }
    if (!triangle.colour.name.empty()) {
        TextureMap texture = TextureMap("../" + triangle.colour.name);
        for (int i = 0; i < 3; ++i) {
            triangle.texturePoints[i].x *= texture.width;
            triangle.texturePoints[i].y *= texture.height;
        }
        Texturing(triangle, texture, window, depthBuffer);
    }else {
        Colour inputColour = triangle.colour;
        rasterCalculate(triangle, inputColour, window, depthBuffer);
    }
}

void renderRasterising(vector<ModelTriangle> triangles, float focalLength, DrawingWindow &window,float scaling_factor) {
    vector<vector<float>> depthBuffer(window.width, vector<float>(window.height, numeric_limits<float>::infinity()));
    for (int i = 0; i < triangles.size(); ++i) {
        Rasterising(triangles[i], focalLength, window, depthBuffer,scaling_factor);
    }
}

void renderTexture(vector<ModelTriangle> triangles, float focalLength, DrawingWindow &window, float scaling_factor) {
    vector<vector<float>> depthBuffer(window.width, vector<float>(window.height, numeric_limits<float>::infinity()));

    for (ModelTriangle& triangle : triangles) {
        if (!triangle.colour.name.empty()) {
            TextureMap texture = TextureMap("../" + triangle.colour.name);
            for (int i = 0; i < 3; ++i) {
                triangle.texturePoints[i].x *= texture.width;
                triangle.texturePoints[i].y *= texture.height;
            }
            Texturing(triangle, texture, window, depthBuffer);
        }
    }
}

vec3 calculateFaceNormal(ModelTriangle &triangle){
    vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
    vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
    vec3 normal = normalize(cross(edge1, edge2));
    return normal;
}

RayTriangleIntersection getClosestValidIntersection(vector<ModelTriangle> &triangles, vec3 rayOrigin, vec3 rayDirection, int depth) {
    RayTriangleIntersection r;
    r.distanceFromCamera = numeric_limits<float>::infinity();
    for (int i = 0; i < triangles.size(); ++i) {
        ModelTriangle triangle = triangles[i];
        vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        vec3 SPVector = rayOrigin - triangle.vertices[0];
        mat3 DEMatrix(-rayDirection, e0, e1);
        vec3 possibleSolution = inverse(DEMatrix) * SPVector;
        float t = possibleSolution.x;
        float u = possibleSolution.y;
        float v = possibleSolution.z;

        vec3 intersectionPoint = rayOrigin + t * rayDirection;
        r.u = u;
        r.v = v;

        if (t > 0 && u >= 0 && u <= 1 && v >= 0 && v <= 1 && (u + v) <= 1) {
            if (r.distanceFromCamera > t) {
                r = RayTriangleIntersection(intersectionPoint, t, triangle, i);
            }
        }
    }
    return r;
}

Colour calculateLighting(RayTriangleIntersection r, vec3 &light) {
    float ambientStrength = 0.2f;
    Colour ambientColour = {
            int(r.intersectedTriangle.colour.red * ambientStrength),
            int(r.intersectedTriangle.colour.green * ambientStrength),
            int(r.intersectedTriangle.colour.blue * ambientStrength)
    };

    vec3 lightDirection = normalize(light - r.intersectionPoint);
    float distance = length(lightDirection);
    float Intensity = 100 / (4 * M_PI * (pow(distance, 2)));
    float proximityLight = std::max(0.0f, std::min(1.0f, Intensity));
    float angleOfIncidence = std::max(0.0f, dot(r.intersectedTriangle.normal, lightDirection));

    // Diffuse lighting
    float DiffuseLight = glm::clamp(proximityLight * angleOfIncidence, 0.2f, 1.0f);

    // Specular highlighting
    vec3 cameraDirection = normalize(cameraPosition - r.intersectionPoint);
    vec3 reflectDirection = normalize(normalize(-lightDirection) - (r.intersectedTriangle.normal * 2.0f * dot(normalize(-lightDirection), r.intersectedTriangle.normal)));
    float reflectionLight = pow(std::max(dot(cameraDirection, reflectDirection), 0.0f), 256.0f);

    // Combine ambient, diffuse, and specular
    Colour finalColour;
    finalColour.red = std::min(int(r.intersectedTriangle.colour.red * DiffuseLight + reflectionLight * 255), 255);
    finalColour.green = std::min(int(r.intersectedTriangle.colour.green * DiffuseLight + reflectionLight * 255), 255);
    finalColour.blue = std::min(int(r.intersectedTriangle.colour.blue * DiffuseLight + reflectionLight * 255), 255);

    return finalColour;
}

vec3 getRayDirectionFromCanvas(int x, int y,int width, int height, float focalLength, float scalingFactor) {
    vec3 rayDirectionWorldSpace;
    float ndcX = (x - cameraPosition.x - (width / 2.0f)) / (focalLength * scalingFactor);
    float ndcY = (y - cameraPosition.y - (height / 2.0f)) / (focalLength * scalingFactor);
    vec3 rayDirectionCameraSpace(-ndcX , -ndcY, -focalLength);
    rayDirectionWorldSpace = normalize(cameraOrientation * rayDirectionCameraSpace);
    return rayDirectionWorldSpace;
}

vector<ModelTriangle> calculateVertexNormals(vector<ModelTriangle> triangles) {
    for (auto& triangle : triangles) {
        triangle.normal = calculateFaceNormal(triangle);
    }
    for (auto& triangle : triangles) {
        for (int v = 0; v < 3; ++v) {
            vec3 vertexNormal = vec3(0.0f);
            int count = 0;
            for (auto& otherTriangle : triangles) {
                for (int vertex = 0; vertex < 3; ++vertex) {
                    if (triangle.vertices[v] == otherTriangle.vertices[vertex]) {
                        vertexNormal += otherTriangle.normal;
                        count++;
                    }
                }
            }
            if (count != 0) {
                triangle.vertexNormals[v] = vertexNormal / static_cast<float>(count);
            }
        }
    }

    return triangles;
}

Colour calculateLightingAtVertex(RayTriangleIntersection r, vec3 normal, float shininess, vec3 sphereLight) {
    float ambientStrength = 0.2f;
    Colour ambientColour = {
            int(r.intersectedTriangle.colour.red * ambientStrength),
            int(r.intersectedTriangle.colour.green * ambientStrength),
            int(r.intersectedTriangle.colour.blue * ambientStrength)
    };

    vec3 lightDirection = normalize(sphereLight - r.intersectionPoint);
    float distance = length(lightDirection);
    float Intensity = 40 / (4 * M_PI * (pow(distance, 2)));
    float proximityLight = std::max(0.0f, std::min(1.0f, Intensity));

    float diffuseLight = std::max(dot(normal, lightDirection), 0.0f);
    Colour diffuseColour = {
            int(r.intersectedTriangle.colour.red * diffuseLight * proximityLight),
            int(r.intersectedTriangle.colour.green * diffuseLight * proximityLight),
            int(r.intersectedTriangle.colour.blue * diffuseLight * proximityLight)
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

vec3 getBarycentricCoordinates(vec3 point, ModelTriangle triangle) {
    vec3 v0 = triangle.vertices[1] - triangle.vertices[0];
    vec3 v1 = triangle.vertices[2] - triangle.vertices[0];
    vec3 v2 = point - triangle.vertices[0];

    vec3 crossProduct = cross(v0, v1);
    float areaABC = length(crossProduct) / 2.0f;
    float areaPBC = length(cross(point - triangle.vertices[1], point - triangle.vertices[2])) / 2.0f;
    float areaPCA = length(cross(point - triangle.vertices[2], point - triangle.vertices[0])) / 2.0f;

    float u = areaPBC / areaABC;
    float v = areaPCA / areaABC;
    float w = 1.0f - u - v;

    return vec3(u, v, w);
}

// soft shadow
float is_shadow(RayTriangleIntersection intersection, vector<ModelTriangle> &triangles, vec3 &light) {
    vec3 shadow_rayDirection = light - intersection.intersectionPoint;
    float shadow_rayLength = length(shadow_rayDirection);
    shadow_rayDirection = normalize(shadow_rayDirection);
    float closestDistance = std::numeric_limits<float>::max();

    for (ModelTriangle &triangle : triangles) {
        vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
        vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
        vec3 SPVector = intersection.intersectionPoint - triangle.vertices[0];
        mat3 DEMatrix(-shadow_rayDirection, e0, e1);
        vec3 possibleSolution = inverse(DEMatrix) * SPVector;
        float t = possibleSolution.x, u = possibleSolution.y, v = possibleSolution.z;
        if ((u >= 0.0) && (u <= 1.0) && (v >= 0.0) && (v <= 1.0) && (u + v) <= 1.0) {
            if (t < shadow_rayLength && t > 0.01 && &triangle != &triangles[intersection.triangleIndex]) {
                closestDistance = std::min(closestDistance, t);
            }
        }
    }
    if (closestDistance < std::numeric_limits<float>::max()) {
        return std::max(0.8f, 0.9f - closestDistance / shadow_rayLength);
    }
    return 0.0f;
}

vector<vec3> getcircleLights() {
    int numLights = 15;
    float radius = 0.05f;
    for (int i = 0; i < numLights; ++i) {
        float angle = 2 * M_PI * i / numLights;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        circleLights.push_back(vec3(x, 0.4f, z + 0.55f));
    }
    return circleLights;
}

Colour calculatePixelLighting( RayTriangleIntersection& r, vec3 normal, int shininess) {
    Colour finalColour = {0, 0, 0};
    for (auto light : circleLights) {
        Colour colour = calculateLightingAtVertex(r, normal, shininess, light);
        finalColour.red += colour.red;
        finalColour.green += colour.green;
        finalColour.blue += colour.blue;
    }
    finalColour.red /= circleLights.size();
    finalColour.green /= circleLights.size();
    finalColour.blue /= circleLights.size();
    return finalColour;
}

Colour Flat(RayTriangleIntersection& r, vec3& rayDirection, vector<ModelTriangle>& triangles, vector<vec3>& lights) {
    r.intersectedTriangle.normal = calculateFaceNormal(r.intersectedTriangle);
    return calculatePixelLighting(r, r.intersectedTriangle.normal, 64.0f);
}

Colour Gouraud(RayTriangleIntersection& r, vec3& rayDirection, vector<ModelTriangle>& triangles, vector<vec3>& lights) {
    r.intersectedTriangle.vertexColours.resize(3);
    for (int v = 0; v < 3; ++v) {
        r.intersectedTriangle.vertexColours[v] = calculatePixelLighting(r, r.intersectedTriangle.vertexNormals[v], 64);
    }

    vec3 barycentric = getBarycentricCoordinates(r.intersectionPoint, r.intersectedTriangle);
    Colour colour;
    colour.red = barycentric.x * r.intersectedTriangle.vertexColours[0].red + barycentric.y * r.intersectedTriangle.vertexColours[1].red + barycentric.z * r.intersectedTriangle.vertexColours[2].red;
    colour.green = barycentric.x * r.intersectedTriangle.vertexColours[0].green + barycentric.y * r.intersectedTriangle.vertexColours[1].green + barycentric.z * r.intersectedTriangle.vertexColours[2].green;
    colour.blue = barycentric.x * r.intersectedTriangle.vertexColours[0].blue + barycentric.y * r.intersectedTriangle.vertexColours[1].blue + barycentric.z * r.intersectedTriangle.vertexColours[2].blue;

    return colour;
}

Colour Phong(RayTriangleIntersection& r, vec3& rayDirection,vector<ModelTriangle>& triangles,vector<vec3>& lights) {
    vec3 barycentric = getBarycentricCoordinates(r.intersectionPoint, r.intersectedTriangle);
    vec3 interpolatedNormal =
            barycentric.x * r.intersectedTriangle.vertexNormals[0] +
            barycentric.y * r.intersectedTriangle.vertexNormals[1] +
            barycentric.z * r.intersectedTriangle.vertexNormals[2];
    interpolatedNormal = normalize(interpolatedNormal);
    return calculatePixelLighting(r, interpolatedNormal, 64.0f);
}

Colour Box(RayTriangleIntersection& r, vec3& rayDirection, vector<ModelTriangle>& triangles, vector<vec3>& lights) {
    if (!isinf(r.distanceFromCamera)) {
        return processRayIntersection(r, rayDirection, triangles, lights, 1);
    } else {
        return {0, 0, 0};
    }
}

void renderScene(DrawingWindow &window, float focalLength, float scaling_factor, vector<ModelTriangle>& triangles, function<Colour(RayTriangleIntersection&, vec3&, vector<ModelTriangle>&, vector<vec3>&)> Types) {
    vector<vec3> rayDirections(window.width * window.height);

    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            rayDirections[y * window.width + x] = getRayDirectionFromCanvas(x, y, window.width, window.height, focalLength, scaling_factor);
        }
    }

    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3& rayDirection = rayDirections[y * window.width + x];
            RayTriangleIntersection r = getClosestValidIntersection(triangles, cameraPosition, rayDirection, 1);
            if (!isinf(r.distanceFromCamera)) {
                Colour pixelColour = Types(r, rayDirection, triangles, circleLights);
                uint32_t finalColor = (255 << 24) + (int(pixelColour.red) << 16) + (int(pixelColour.green) << 8) + int(pixelColour.blue);
                window.setPixelColour(x, y, finalColor);
            }
        }
    }
}


float calculateFresnelEffect(vec3 &I, vec3 &N, float refractiveIndex) {
    float cosTheta = -dot(normalize(I), normalize(N));
    float R0 = pow((1.0f - refractiveIndex) / (1.0f + refractiveIndex), 2);
    return R0 + (1 - R0) * pow(1 - cosTheta, 5);
}

Colour mixColours(Colour &reflectionColour, Colour &refractionColour, float fresnelEffect) {
    Colour mixedColour;
    mixedColour.red = reflectionColour.red * fresnelEffect + refractionColour.red * (1 - fresnelEffect);
    mixedColour.green = reflectionColour.green * fresnelEffect + refractionColour.green * (1 - fresnelEffect);
    mixedColour.blue = reflectionColour.blue * fresnelEffect + refractionColour.blue * (1 - fresnelEffect);

    mixedColour.red = std::min(std::max(mixedColour.red, 0), 255);
    mixedColour.green = std::min(std::max(mixedColour.green, 0), 255);
    mixedColour.blue = std::min(std::max(mixedColour.blue, 0), 255);
    return mixedColour;
}

Colour handleMirrorReflection(RayTriangleIntersection r, vec3 reflectionDirection, vector<ModelTriangle> triangles, vector<vec3> Mult_lights, int depth) {
    vec3 offset = 0.001f * r.intersectedTriangle.normal;
    RayTriangleIntersection reflectionIntersection = getClosestValidIntersection(triangles,
                                                                                 r.intersectionPoint + offset, reflectionDirection,
                                                                                 depth + 1);
    if (!isinf(reflectionIntersection.distanceFromCamera)) {
        return processRayIntersection(reflectionIntersection, reflectionDirection, triangles, Mult_lights, depth + 1);
    }
    return {0, 0, 0};
}

Colour handleGlass(RayTriangleIntersection& r, vec3& rayDirection, vec3& normal, vector<ModelTriangle> &triangles, vector<vec3> &Mult_lights, int depth) {
    vec3 refractedDirection = refract(rayDirection, normal, 1.0f);
    if (!isnan(refractedDirection.x)) {
        vec3 refractedRayOrigin = r.intersectionPoint + refractedDirection * 0.001f;
        RayTriangleIntersection refractionIntersection = getClosestValidIntersection(triangles, refractedRayOrigin, refractedDirection, depth + 1);
        Colour refractionColour = {0, 0, 0};
        if (!isinf(refractionIntersection.distanceFromCamera)) {
            refractionColour = processRayIntersection(refractionIntersection, refractedDirection, triangles, Mult_lights, depth + 1);
        }

        vec3 reflectionDirection = reflect(rayDirection, normal);
        RayTriangleIntersection reflectionIntersection = getClosestValidIntersection(triangles, r.intersectionPoint + normal * 0.001f, reflectionDirection, depth + 1);
        Colour reflectionColour = {0, 0, 0};
        if (!isinf(reflectionIntersection.distanceFromCamera)) {
            reflectionColour = processRayIntersection(reflectionIntersection, reflectionDirection, triangles, Mult_lights, depth + 1);
        }
        float fresnelEffect = calculateFresnelEffect(rayDirection, normal, 1.0f);
        return mixColours(reflectionColour, refractionColour, fresnelEffect);
    }
    return r.intersectedTriangle.colour;
}

Colour handleDiffuse(RayTriangleIntersection r, vector<ModelTriangle> triangles, vector<vec3> &Mult_lights) {
    Colour accumulatedColour = {0, 0, 0};
    for (int l = 0; l < Mult_lights.size(); ++l) {
        float shadowIntensity = is_shadow(r, triangles, Mult_lights[l]);
        Colour colour = calculateLighting(r, Mult_lights[l]);
        accumulatedColour.red += colour.red * (1.0f - shadowIntensity);
        accumulatedColour.green += colour.green * (1.0f - shadowIntensity);
        accumulatedColour.blue += colour.blue * (1.0f - shadowIntensity);
    }
    if (Mult_lights.size() > 0) {
        accumulatedColour.red /= Mult_lights.size();
        accumulatedColour.green /= Mult_lights.size();
        accumulatedColour.blue /= Mult_lights.size();
    }
    accumulatedColour.red = std::min(255.0f, float(accumulatedColour.red));
    accumulatedColour.green = std::min(255.0f, float(accumulatedColour.green));
    accumulatedColour.blue = std::min(255.0f, float(accumulatedColour.blue));
    return accumulatedColour;
}

Colour processRayIntersection(RayTriangleIntersection r, vec3 rayDirection, vector<ModelTriangle> triangles, vector<vec3> Mult_lights, int depth) {
    Colour accumulatedColour = {0, 0, 0};
    if (depth >= 6) {
        return accumulatedColour;
    }
    vec3 normal = calculateFaceNormal(r.intersectedTriangle);
    if (r.intersectedTriangle.isMirror) {
        vec3 reflectionDirection = reflect(rayDirection, normal);
        accumulatedColour = handleMirrorReflection(r, reflectionDirection, triangles, Mult_lights, depth);
    }
    else if (r.intersectedTriangle.isGlass) {
        if (dot(rayDirection, normal) > 0) {
            normal = -normal;
        }
        accumulatedColour = handleGlass(r, rayDirection, normal, triangles, Mult_lights, depth);
    }
    else {
        accumulatedColour = handleDiffuse(r, triangles,Mult_lights);
    }
    return accumulatedColour;
}

void renderRayTracedScene(vector<ModelTriangle> triangles, DrawingWindow &window, float focalLength, float scaling_factor) {
    vector<vec3> Mult_lights = getcircleLights();
    vector<ModelTriangle> sphereTriangles;
    vector<ModelTriangle> sphereTriangles2;
    vector<ModelTriangle> sphereTriangles3;
    vector<ModelTriangle> otherTriangles;

    for (ModelTriangle& triangle : triangles) {
        if (triangle.name == "sphere") {
            sphereTriangles.push_back(triangle);
        } else if(triangle.name == "sphere2"){
            sphereTriangles2.push_back(triangle);

        } else if(triangle.name == "sphere3"){
            sphereTriangles3.push_back(triangle);
        } else {
            otherTriangles.push_back(triangle);
        }
    }

    if (!otherTriangles.empty()) {
        renderScene(window, focalLength, scaling_factor, triangles, Box);
    }
    if(!sphereTriangles.empty()) {
        renderScene(window, focalLength, scaling_factor, sphereTriangles, Flat);
    }
    if(!sphereTriangles2.empty()){
        renderScene(window, focalLength, scaling_factor, sphereTriangles2, Phong);
    }
    if(!sphereTriangles3.empty()){
        renderScene(window, focalLength, scaling_factor, sphereTriangles3, Gouraud);
    }
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
        else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
        else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
        else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        window.savePPM("output.ppm");
        window.saveBMP("output.bmp");
    }
}

void look_At() {
    vec3 forward = normalize(vec3(0.0,0.0,0.0) - cameraPosition);
    vec3 right = normalize(cross(vec3(0.0,1.0,0.0),forward));
    vec3 up = normalize(cross(forward,right));

    cameraOrientation[0] = right;
    cameraOrientation[1] = up;
    cameraOrientation[2] = -forward;
}

void orbit() {
    if (orbiting) {
        cameraPosition = cameraPosition * mat3(
                vec3( cos(-M_PI/180), 0.0, sin(-M_PI/180)),
                vec3(0.0, 1.0,    0.0),
                vec3(-sin(-M_PI/180), 0.0, cos(-M_PI/180))
        );
        cameraOrientation = cameraOrientation * mat3(
                vec3( cos(-M_PI/180), 0.0, sin(-M_PI/180)),
                vec3(0.0, 1.0,    0.0),
                vec3(-sin(-M_PI/180), 0.0, cos(-M_PI/180))
        );
        look_At();
    }
}

void ResetCamera() {
    cameraPosition = vec3(0.0,0.0,4.0);
    cameraOrientation = mat3(
            vec3(-1.0,0.0,0.0),
            vec3(0.0,1.0,0.0),
            vec3(0.0,0.0,1.0));
}

vector<ModelTriangle> loadObjFile(std::string filename, float scalingFactor, unordered_map<string,Colour> colours) {
    ifstream file(filename);
    vector<ModelTriangle> triangles;
    vector<vec3> vertices;
    vector<TexturePoint> texturePoints;
    vector<vec3> normals;
    string line;
    string currentMaterial;
    string colour;
    bool glass = false;
    bool mirror = false;
    string currentObjectName;

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return triangles;;
    }
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> tokens = split(line, ' ');
        if (tokens[0] == "o"){
            currentObjectName = tokens[1];
        }
        if (tokens[0] == "v") {
            vec3 vertex;
            vertex.x = std::stof(tokens[1]);
            vertex.y = std::stof(tokens[2]);
            vertex.z = std::stof(tokens[3]);
            vertex *= scalingFactor;
            vertices.push_back(vertex);
        }
        if (tokens[0] == "vt") {
            TexturePoint t;
            t.x = stof(tokens[1]);
            t.y = stof(tokens[2]);
            texturePoints.push_back(t);
        } else if (tokens[0] == "f") {
            if (tokens.size() < 4) continue;
            vector<string> v1_tokens = split(tokens[1], '/');
            vector<string> v2_tokens = split(tokens[2], '/');
            vector<string> v3_tokens = split(tokens[3], '/');
            if (v1_tokens.empty() || v2_tokens.empty() || v3_tokens.empty()) continue;

            int v1 = stoi(v1_tokens[0]) - 1;
            int v2 = stoi(v2_tokens[0]) - 1;
            int v3 = stoi(v3_tokens[0]) - 1;

            ModelTriangle triangle(vertices[v1], vertices[v2], vertices[v3], colours[currentMaterial]);
            if (!normals.empty()) {
                triangle.vertexNormals[0] = normals[stoi(v1_tokens[2]) - 1];
                triangle.vertexNormals[1] = normals[stoi(v2_tokens[2]) - 1];
                triangle.vertexNormals[2] = normals[stoi(v3_tokens[2]) - 1];
            }
            triangle.normal = cross(vec3(triangle.vertices[1] - triangle.vertices[0]),
                                    vec3(triangle.vertices[2] - triangle.vertices[0]));
            if (v1_tokens.size() > 1 && v1_tokens[1] != "" &&
                v2_tokens.size() > 1 && v2_tokens[1] != "" &&
                v3_tokens.size() > 1 && v3_tokens[1] != "") {
                int vt1 = stoi(v1_tokens[1]) - 1;
                int vt2 = stoi(v2_tokens[1]) - 1;
                int vt3 = stoi(v3_tokens[1]) - 1;

                triangle.texturePoints[0] = texturePoints[vt1];
                triangle.texturePoints[1] = texturePoints[vt2];
                triangle.texturePoints[2] = texturePoints[vt3];
            }
            triangle.isMirror = mirror;
            triangle.name = currentObjectName;
            triangle.isGlass = glass;
            triangles.push_back(triangle);
        }
        if (tokens[0] == "usemtl") {
            mirror = (tokens[1] == "Mirror");
            glass = (tokens[1] == "Glass");
            currentMaterial = tokens[1];
        }
        if (tokens[0] == "vn") {
            vec3 normal;
            normal.x = std::stof(tokens[1]);
            normal.y = std::stof(tokens[2]);
            normal.z = std::stof(tokens[3]);
            normals.push_back(normal);
        }
    }
    file.close();
    return triangles;
}

unordered_map<string,Colour> loadMtlFile(std::string filename) {
    ifstream file(filename);
    string line;
    string material;
    unordered_map<string,Colour> colours;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> token = split(line,' ');
        if (token[0]=="newmtl"){
            material = token[1];
        }
        if(token[0]=="Kd"){
            float r = std::stof(token[1]) ;
            float g = std::stof(token[2]) ;
            float b = std::stof(token[3]) ;
            int red = static_cast<int>(r * 255);
            int green = static_cast<int>(g * 255);
            int blue = static_cast<int>(b * 255);
            colours.insert({material, Colour(red, green, blue)});
        }
        if(token[0]=="map_Kd"){
            Colour colour = colours[material];
            colour.name = token[1];
            colours[material] = colour;
        }
    }
    file.close();
    return colours;
}

void switchModes(DrawingWindow &window,SDL_Event event){
    float translationAmount = 0.1f;
    float scalingFactor = 0.35f;
    float focalLength = 2.0f;
    vector<ModelTriangle> wireFrameTriangle = loadObjFile("../cornell-box.obj",scalingFactor,loadMtlFile("../cornell-box.mtl"));
    vector<ModelTriangle> filledTriangle = loadObjFile("../cornell-box.obj",scalingFactor,loadMtlFile("../cornell-box.mtl"));
    vector<ModelTriangle> mirrorTriangle =  loadObjFile("../rabbit.obj",scalingFactor,loadMtlFile("../cornell-box.mtl"));
    vector<ModelTriangle> sphere = loadObjFile("../Sphere.obj",scalingFactor,loadMtlFile("../cornell-box.mtl"));
    vector<ModelTriangle> textureTriangle = loadObjFile("../textured-cornell-box.obj",scalingFactor,loadMtlFile("../textured-cornell-box.mtl"));
    vector<ModelTriangle> logoTriangle = loadObjFile("../logo.obj", scalingFactor,loadMtlFile("../materials.mtl"));

    if (event.type == SDL_KEYDOWN) {
        // Camera translation
        if (event.key.keysym.sym == SDLK_a) cameraPosition.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_d) cameraPosition.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_w) cameraPosition.y -= translationAmount;
        else if (event.key.keysym.sym == SDLK_s) cameraPosition.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_q) cameraPosition.z += translationAmount;
        else if (event.key.keysym.sym == SDLK_e) cameraPosition.z -= translationAmount;

        //Camera rotation
        if (event.key.keysym.sym == SDLK_UP) {
            cameraPosition = cameraPosition * mat3(
                    vec3( 1.0,    0.0,    0.0),
                    vec3( 0.0, cos(M_PI/180),-sin(M_PI/180)),
                    vec3( 0.0, sin(M_PI/180), cos(M_PI/180))
            );
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            cameraPosition = cameraPosition * mat3(
                    vec3( 1.0,    0.0,    0.0),
                    vec3( 0.0, cos(-M_PI/180),-sin(-M_PI/180)),
                    vec3( 0.0, sin(-M_PI/180), cos(-M_PI/180))
            );
        }
        else if (event.key.keysym.sym == SDLK_LEFT) {
            cameraPosition = cameraPosition * mat3(
                    vec3( cos(M_PI/180), 0.0, sin(M_PI/180)),
                    vec3(0.0, 1.0,    0.0),
                    vec3(-sin(M_PI/180), 0.0, cos(M_PI/180))
            );
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            cameraPosition = cameraPosition * mat3(
                    vec3( cos(-M_PI/180), 0.0, sin(-M_PI/180)),
                    vec3(0.0, 1.0,    0.0),
                    vec3(-sin(-M_PI/180), 0.0, cos(-M_PI/180))
            );
        }
        // Camera orientation rotation
        if (event.key.keysym.sym == SDLK_l) {
            cameraOrientation = cameraOrientation * mat3(
                    vec3( cos(-M_PI/180), 0.0, sin(-M_PI/180)),
                    vec3(0.0, 1.0,    0.0),
                    vec3(-sin(-M_PI/180), 0.0, cos(-M_PI/180))
            );
        }
        else if (event.key.keysym.sym == SDLK_j) {
            cameraOrientation = cameraOrientation * mat3(
                    vec3( cos(M_PI/180), 0.0, sin(M_PI/180)),
                    vec3(0.0, 1.0,    0.0),
                    vec3(-sin(M_PI/180), 0.0, cos(M_PI/180))
            );
        }
        else if (event.key.keysym.sym == SDLK_k) {
            cameraOrientation = cameraOrientation * mat3(
                    vec3( 1.0,    0.0,    0.0),
                    vec3( 0.0, cos(-M_PI/180),-sin(-M_PI/180)),
                    vec3( 0.0, sin(-M_PI/180), cos(-M_PI/180))
            );
        }
        else if (event.key.keysym.sym == SDLK_i)  {
            cameraOrientation = cameraOrientation * mat3(
                    vec3( 1.0,    0.0,    0.0),
                    vec3( 0.0, cos(M_PI/180),-sin(M_PI/180)),
                    vec3( 0.0, sin(M_PI/180), cos(M_PI/180))
            );
        }
        for (int i = 0; i < circleLights.size(); ++i) {
            if (event.key.keysym.sym == SDLK_x) circleLights[i].x += translationAmount;
            else if (event.key.keysym.sym == SDLK_z) circleLights[i].x -= translationAmount;
            else if (event.key.keysym.sym == SDLK_b) circleLights[i].y -= translationAmount;
            else if (event.key.keysym.sym == SDLK_n) circleLights[i].y += translationAmount;
            else if (event.key.keysym.sym == SDLK_c) circleLights[i].z += translationAmount;
            else if (event.key.keysym.sym == SDLK_v) circleLights[i].z -= translationAmount;
        }
        if (event.key.keysym.sym == SDLK_h) look_At();
        if (event.key.keysym.sym == SDLK_o) orbiting = !orbiting;
        if (event.key.keysym.sym == SDLK_r) ResetCamera();
        if (event.key.keysym.sym == SDLK_1) {
            currentRenderMode = RENDER_WIREFRAME;
        }
        else if (event.key.keysym.sym == SDLK_2) {
            currentRenderMode = RENDER_RASTERISED;
        }
        else if (event.key.keysym.sym == SDLK_3) {
            currentRenderMode = RENDER_Texture;
        }
        else if (event.key.keysym.sym == SDLK_4) {
            currentRenderMode = RENDER_RayTRACED;
        }
        else if (event.key.keysym.sym == SDLK_5) {
            currentRenderMode = RENDER_FlatSphere;
        }
        else if (event.key.keysym.sym == SDLK_6) {
            currentRenderMode = RENDER_GOURAUD;
        }
        else if (event.key.keysym.sym == SDLK_7) {
            currentRenderMode = RENDER_PHONG;
        }
        else if (event.key.keysym.sym == SDLK_8) {
            currentRenderMode = RENDER_LOGO;
        }
    }
    window.clearPixels();
    switch (currentRenderMode) {
        case RENDER_WIREFRAME:
            renderWireframe(wireFrameTriangle,window,focalLength,300.0f);
            break;
        case RENDER_RASTERISED:
            renderRasterising(filledTriangle,focalLength,window,300.0f);
            break;
        case RENDER_Texture:
            renderRasterising(textureTriangle,focalLength,window,300.0f);
            break;
        case RENDER_RayTRACED:
            renderRayTracedScene(calculateVertexNormals(mirrorTriangle),window,focalLength,180.0f);
            break;
//        case RENDER_FlatSphere:
//            renderFlatSphere(sphere,window,focalLength,300.0f);
//        case RENDER_GOURAUD:
//            gouraud(calculateVertexNormals(sphere),window,focalLength,300.0f);
//            break;
//        case RENDER_PHONG:
//            phong(calculateVertexNormals(sphere),window,focalLength,300.0f);
//            break;
        case RENDER_LOGO:
            renderTexture(logoTriangle,focalLength,window,2.0f);
            break;
        case RENDER_NONE:
            break;
    }
}

void renderFrame(vector<ModelTriangle> triangles, DrawingWindow &window, float focalLength) {
    int frameCount = 96;
    int n_zero = 5;
//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        renderWireframe(triangles, window, focalLength, 400.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//        float speed = -3.8 * M_PI / 180;
//        cameraPosition = cameraPosition * mat3(
//                vec3(cos(speed), 0.0, sin(speed)),
//                vec3(0.0, 1.0, 0.0),
//                vec3(-sin(speed), 0.0, cos(speed))
//        );
//        cameraOrientation = cameraOrientation * mat3(
//                vec3(cos(speed), 0.0, sin(speed)),
//                vec3(0.0, 1.0, 0.0),
//                vec3(-sin(speed), 0.0, cos(speed))
//        );
//        look_At();
//    }
//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        renderRasterising(triangles, focalLength, window, 400.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output3/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//        if(frame < 12) {
//            cameraPosition.x -= 0.06;
//        }
//        else if(frame < 36) {
//            cameraPosition.x += 0.06;
//        }
//        else if(frame < 48) {
//            cameraPosition.x -= 0.06;
//        }
//    }

//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        renderRasterising(triangles, focalLength, window, 400.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output4/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//        if(frame < 12) {
//            cameraPosition.y -= 0.06;
//        }
//        else if(frame < 36) {
//            cameraPosition.y += 0.06;
//        }
//        else if(frame < 48) {
//            cameraPosition.y -= 0.06;
//        }
//    }
//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        renderRasterising(triangles, focalLength, window, 400.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output2/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//        if(frame < 12) {
//            cameraPosition.z -= 0.06;
//        }
//        else if(frame < 36) {
//            cameraPosition.z += 0.06;
//        }
//        else if(frame < 48) {
//            cameraPosition.z -= 0.06;
//        }
//    }
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

//    vector<ModelTriangle> wireFrameTriangle = loadObjFile("../textured-cornell-box.obj",0.35f,loadMtlFile("../cornell-box.mtl"));
//    vector<ModelTriangle> mirrorTriangle =  loadObjFile("../rabbit.obj",0.35f,loadMtlFile("../cornell-box.mtl"));
//    renderFrame(calculateVertexNormals(mirrorTriangle),window,2.0f);
//    renderFrame(wireFrameTriangle,window,2.0f);

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
        switchModes(window,event);
        orbit();
//		draw(window);
		window.renderFrame();
	}
}
