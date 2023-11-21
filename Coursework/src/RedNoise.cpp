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
mat3 cameraOrientation(
        vec3(-1.0,0.0,0.0),
        vec3(0.0, 1.0,0.0),
        vec3(0.0,0.0,1.0)
);
vec3 cameraPosition(0.0f, 0.0f, 4.0f);
vec3 BoxLight(0.0,0.4f,0.3f);
vec3 SphereLight(0.0f,1.5f, 3.5f);

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

void draw(DrawingWindow &window) {
    window.clearPixels();
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

vector<ModelTriangle> loadObjFile(const std::string& filename, float scalingFactor, unordered_map<string,Colour> colours) {
    ifstream file(filename);
    vector<ModelTriangle> triangles;
    vector<vec3> vertices;
    vector<TexturePoint> texturePoints;

    if(!file.is_open()){
        cerr<<"Error: Could not open file "<<filename<<endl;
        return triangles;;
    }

    string line;
    string currentMaterial;
    string colour;

    while(getline(file,line)){
        if (line.empty()) {
            continue;
        }
        std::vector<std::string> tokens = split(line, ' ');
        if (tokens[0] == "v") {
            vec3 vertex;
            vertex.x= std::stof(tokens[1]);
            vertex.y = std::stof(tokens[2]);
            vertex.z = std::stof(tokens[3]);
            vertex *= scalingFactor;
            vertices.push_back(vertex);
        }
        if (tokens[0] == "vt"){
            TexturePoint t;
            t.x = stof(tokens[1]);
            t.y = stof(tokens[2]);
            texturePoints.push_back(t);
        }
        else if (tokens[0] == "f") {
            if(tokens.size() < 4) continue; // Not a valid face definition.
            vector<string> v1_tokens = split(tokens[1], '/');
            vector<string> v2_tokens = split(tokens[2], '/');
            vector<string> v3_tokens = split(tokens[3], '/');

            if(v1_tokens.empty() || v2_tokens.empty() || v3_tokens.empty()) continue;

            int v1 = stoi(v1_tokens[0]) - 1;
            int v2 = stoi(v2_tokens[0]) - 1;
            int v3 = stoi(v3_tokens[0]) - 1;

            ModelTriangle triangle(vertices[v1], vertices[v2], vertices[v3], colours[currentMaterial]);

            if (v1_tokens.size() > 1 && v1_tokens[1] != "" &&
                v2_tokens.size() > 1 && v2_tokens[1] != "" &&
                v3_tokens.size() > 1 && v3_tokens[1] != "")
            {
                int vt1 = stoi(v1_tokens[1]) - 1;
                int vt2 = stoi(v2_tokens[1]) - 1;
                int vt3 = stoi(v3_tokens[1]) - 1;

                triangle.texturePoints[0] = texturePoints[vt1];
                triangle.texturePoints[1] = texturePoints[vt2];
                triangle.texturePoints[2] = texturePoints[vt3];
            }
            triangles.push_back(triangle);
        }else if (tokens[0] == "usemtl") {
            currentMaterial = tokens[1];
        }
    }
    file.close();
    return triangles;
}

unordered_map<string,Colour> loadMtlFile(const std::string& filename) {
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

void renderTexture(ModelTriangle triangle, TextureMap texture, DrawingWindow &window, vector<vector<float>>& depthBuffer) {
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
                        uint32_t index = int(texY) * texture.width + int(texX);
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

void Rasterising(ModelTriangle triangle,float focalLength, DrawingWindow &window, vector<vector<float>>& depthBuffer,float scaling_factor) {
    RayTriangleIntersection r;
    for (int i = 0; i < 3; ++i) {
        vec3 vertexPosition = triangle.vertices[i];
        vec2 canvasPoint = getCanvasIntersectionPoint(vertexPosition, focalLength, scaling_factor, window.width, window.height);
        vec3 cameraSpaceVertex = cameraOrientation * (vertexPosition - cameraPosition);
        triangle.vertices[i] = vec3(canvasPoint.x, canvasPoint.y, -cameraSpaceVertex.z);
    }
    if (!triangle.colour.name.empty()) {
        TextureMap texture = TextureMap("../" + triangle.colour.name);
        for (int i = 0; i < 3; ++i) {
            triangle.texturePoints[i].x *= texture.width;
            triangle.texturePoints[i].y *= texture.height;
        }
        renderTexture(triangle, texture, window, depthBuffer);
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
            renderTexture(triangle, texture, window, depthBuffer);
        }
    }
}

RayTriangleIntersection getClosestValidIntersection(vector<ModelTriangle> &triangles, vec3 rayDirection) {
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

Colour calculateLighting(RayTriangleIntersection r, vec3 &light) {
    float ambientStrength = 0.2f;
    Colour ambientColour = {
            int(r.intersectedTriangle.colour.red * ambientStrength),
            int(r.intersectedTriangle.colour.green * ambientStrength),
            int(r.intersectedTriangle.colour.blue * ambientStrength)
    };

    vec3 lightDirection = normalize(light - r.intersectionPoint);
    float distance = length(lightDirection);
    float Intensity = 40 / (4 * M_PI * (pow(distance, 2)));
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

vec3 getRayDirectionFromCanvas(int x, int y,int width, int height, float focalLength, float scalingFactor) {
    vec3 rayDirectionWorldSpace;
    float ndcX = (x - cameraPosition.x - (width / 2.0f)) / (focalLength * scalingFactor);
    float ndcY = (y - cameraPosition.y - (height / 2.0f)) / (focalLength * scalingFactor);
    vec3 rayDirectionCameraSpace(-ndcX , -ndcY, -focalLength);
    rayDirectionWorldSpace = normalize(cameraOrientation * rayDirectionCameraSpace);
    return rayDirectionWorldSpace;
}

vec3 calculateFaceNormal(ModelTriangle &triangle){
    vec3 edge1 = triangle.vertices[1] - triangle.vertices[0];
    vec3 edge2 = triangle.vertices[2] - triangle.vertices[0];
    vec3 normal = normalize(cross(edge1, edge2));
    return normal;
}

void renderRayTracedScene(vector<ModelTriangle> triangles, DrawingWindow &window, float focalLength, float scaling_factor) {
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x,y,window.width,window.height,focalLength,scaling_factor);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, rayDirection);
            if (!isinf(r.distanceFromCamera)) {
                r.intersectedTriangle.normal = calculateFaceNormal(r.intersectedTriangle);
                Colour colour = calculateLighting(r,BoxLight);
                uint32_t ray_Color = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                if (!is_shadow(r, triangles, BoxLight)) {
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

void renderFlatSphere(vector<ModelTriangle> triangles, DrawingWindow &window, float focalLength, float scaling_factor) {
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x,y,window.width,window.height,focalLength,scaling_factor);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, rayDirection);
            if (!isinf(r.distanceFromCamera)) {
                r.intersectedTriangle.normal = calculateFaceNormal(r.intersectedTriangle);
                Colour colour = calculateLighting(r,SphereLight);
                uint32_t ray_Color = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                    window.setPixelColour(x, y, ray_Color);
            }
        }
    }
}

vector<ModelTriangle> calculateVertexNormals(vector<ModelTriangle> triangles) {
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

Colour calculateLightingAtVertex(RayTriangleIntersection r, vec3 normal, float shininess ) {
    float ambientStrength = 0.2f;
    Colour ambientColour = {
            int(r.intersectedTriangle.colour.red * ambientStrength),
            int(r.intersectedTriangle.colour.green * ambientStrength),
            int(r.intersectedTriangle.colour.blue * ambientStrength)
    };

    vec3 lightDirection = normalize(SphereLight - r.intersectionPoint);
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

void gouraud(vector<ModelTriangle> triangles, DrawingWindow &window, float focalLength, float scaling_factor) {
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x, y, window.width, window.height, focalLength, scaling_factor);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, rayDirection);
            for (auto& triangle : triangles) {
                triangle.vertexColours.resize(triangle.vertices.size());
                for (int v = 0; v < triangle.vertices.size(); ++v) {
                    triangle.vertexColours[v] = calculateLightingAtVertex(r,triangle.vertexNormals[v],64.0f);
                }
            }
            if (!isinf(r.distanceFromCamera)) {
                vec3 barycentric = getBarycentricCoordinates(r.intersectionPoint, r.intersectedTriangle);
                Colour colour;
                colour.red = barycentric.x * r.intersectedTriangle.vertexColours[0].red + barycentric.y * r.intersectedTriangle.vertexColours[1].red + barycentric.z * r.intersectedTriangle.vertexColours[2].red;
                colour.green = barycentric.x * r.intersectedTriangle.vertexColours[0].green + barycentric.y * r.intersectedTriangle.vertexColours[1].green + barycentric.z * r.intersectedTriangle.vertexColours[2].green;
                colour.blue = barycentric.x * r.intersectedTriangle.vertexColours[0].blue + barycentric.y * r.intersectedTriangle.vertexColours[1].blue + barycentric.z * r.intersectedTriangle.vertexColours[2].blue;

                uint32_t sphere_Color = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                window.setPixelColour(x, y, sphere_Color);
            }
        }
    }
}

void phong(vector<ModelTriangle> triangles, DrawingWindow &window, float focalLength, float scaling_factor) {
    for (int y = 0; y < window.height; y++) {
        for (int x = 0; x < window.width; x++) {
            vec3 rayDirection = getRayDirectionFromCanvas(x, y, window.width, window.height, focalLength, scaling_factor);
            RayTriangleIntersection r = getClosestValidIntersection(triangles, rayDirection);
            if (!isinf(r.distanceFromCamera)) {
                vec3 barycentric = getBarycentricCoordinates(r.intersectionPoint, r.intersectedTriangle);
                vec3 interpolatedNormal =
                        barycentric.x * r.intersectedTriangle.vertexNormals[0] +
                        barycentric.y * r.intersectedTriangle.vertexNormals[1] +
                        barycentric.z * r.intersectedTriangle.vertexNormals[2];
                interpolatedNormal = normalize(interpolatedNormal);
                Colour colour = calculateLightingAtVertex(r, interpolatedNormal, 64.0f);
                uint32_t finalColor = (255 << 24) + (int(colour.red) << 16) + (int(colour.green) << 8) + int(colour.blue);
                window.setPixelColour(x, y, finalColor);
            }
        }
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

void switchModes(DrawingWindow &window,SDL_Event event){
    float translationAmount = 0.1f;
    float scalingFactor = 0.35f;
    float focalLength = 2.0f;
    vector<ModelTriangle> wireFrameTriangle = loadObjFile("../cornell-box.obj",scalingFactor,loadMtlFile("../cornell-box.mtl"));
    vector<ModelTriangle> filledTriangle = loadObjFile("../cornell-box.obj",scalingFactor,loadMtlFile("../cornell-box.mtl"));
    vector<ModelTriangle> sphere = loadObjFile("../sphere.obj",scalingFactor,loadMtlFile("../textured-cornell-box.mtl"));
    vector<ModelTriangle> textureTriangle = loadObjFile("../textured-cornell-box.obj",scalingFactor,loadMtlFile("../textured-cornell-box.mtl"));
    vector<ModelTriangle> logoTriangle = loadObjFile("../logo.obj", scalingFactor ,loadMtlFile("../materials.mtl"));

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
        if (event.key.keysym.sym == SDLK_x) BoxLight.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_z) BoxLight.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_b) BoxLight.y -= translationAmount;
        else if (event.key.keysym.sym == SDLK_n) BoxLight.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_c) BoxLight.z += translationAmount;
        else if (event.key.keysym.sym == SDLK_v) BoxLight.z -= translationAmount;
        else if (event.key.keysym.sym == SDLK_h) look_At();
        else if (event.key.keysym.sym == SDLK_o) orbiting = !orbiting;
        else if (event.key.keysym.sym == SDLK_r) ResetCamera();
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
            renderRayTracedScene(filledTriangle,window,focalLength,180.0f);
            break;
        case RENDER_FlatSphere:
            renderFlatSphere(sphere,window,focalLength,300.0f);
            break;
        case RENDER_GOURAUD:
            gouraud(calculateVertexNormals(sphere),window,focalLength,300.0f);
            break;
        case RENDER_PHONG:
            phong(calculateVertexNormals(sphere),window,focalLength,300.0f);
            break;
        case RENDER_LOGO:
            renderTexture(logoTriangle,focalLength,window,2.0f);
            break;
        case RENDER_NONE:
            break;
    }
}


//void renderFrame(vector<ModelTriangle> &triangles, DrawingWindow &window, float focalLength) {
//    const int frameCount = 48;
//    const int n_zero = 5;
//    const float cameraMoveStep = 0.075f;
//
//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        drawWireframe(triangles, cameraPosition, focalLength, window, cameraOrientation, 500.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//    }
//
//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        drawRasterisedModel(triangles, cameraPosition, focalLength, window, cameraOrientation, 500.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output1/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//    }
//
//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        drawRayTracedScene(triangles,window,cameraPosition,focalLength,cameraOrientation,BoxLight,300.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output2/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//    }
//
//}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

//    vector<string> filenames = {"../cornell-box.obj", "../sphere.obj"};
//    unordered_map<string, vector<ModelTriangle>> allTriangles = loadMultipleObjFiles(filenames, 0.35f, loadMtlFile("../cornell-box.mtl"));
//    vector<ModelTriangle> box = allTriangles["../cornell-box.obj"];
//    vector<ModelTriangle> sphere = allTriangles["../sphere.obj"];
//    renderFrame(box,window,2.0f);

	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
        switchModes(window,event);
        orbit();
//		draw(window);

		window.renderFrame();
	}
}
