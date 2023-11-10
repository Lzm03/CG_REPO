#pragma once

#include <fstream>
#include <ModelTriangle.h>
#include <glm/glm.hpp>
#include <Utils.h>
#include "DrawFile.h"

using namespace std;
using namespace glm;

//vector<pair<string, Colour>> palette;

//vector<ModelTriangle> loadObjFile(const std::string& filename, float scalingFactor, string textureFile){
//    ifstream file(filename);
//    vector<ModelTriangle> triangles;
//    vector<vec3> vertices;
//    vector<TexturePoint> texturePoints;
//
//    if(!file.is_open()){
//        cerr<<"Error: Could not open file "<<filename<<endl;
//        return triangles;;
//    }
//
//    string line;
//    vector<pair<string, Colour>> palette;
//    loadMtlFile("../textured-cornell-box.mtl",palette,textureFile);
//    string currentMaterial;
//
//    while(getline(file,line)){
//        if (line.empty()) {
//            continue;
//        }
//        std::vector<std::string> tokens = split(line, ' ');
//        if (tokens[0] == "v") {
//            vec3 vertex;
//            vertex.x= std::stof(tokens[1]);
//            vertex.y = std::stof(tokens[2]);
//            vertex.z = std::stof(tokens[3]);
//            vertex *= scalingFactor;
//            vertices.push_back(vertex);
//            //triangles.push_back(ModelTriangle(triangles.back(vertex.x),vertex.y,vertex.z,Colour{255,255,255}));
//        }
//        if (tokens[0] == "vt"){
//           TexturePoint t;
//           t.x = stof(tokens[1]);
//           t.y = stof(tokens[2]);
//           texturePoints.push_back(t);
//        }
//        if (tokens[0] == "f") {
//            vector<string> parts = split(line, '/');
//            int v1, v2, v3;
//            v1 = std::stoi(tokens[1]) - 1;
//            v2 = std::stoi(tokens[2]) - 1;
//            v3 = std::stoi(tokens[3]) - 1;
//
//            Colour triangleColour; // Default colour
//            for (const auto& data : palette) {
//                if (data.first == currentMaterial) {
//                    triangleColour = data.second;
//                    cout<<currentMaterial<<endl;
//                    break;
//                }
//            }
//
//            ModelTriangle triangle;
//            triangle = ModelTriangle(vertices[v1], vertices[v2], vertices[v3], triangleColour);
//
//            triangles.push_back(triangle);
//        }
//        if(tokens[0] == "usemtl"){
//            currentMaterial = tokens[1];
//        }
//    }
//    file.close();
//    return triangles;
//}

//vector<ModelTriangle> loadObjFile(const std::string& filename, float scalingFactor, string textureFile) {
//    ifstream file(filename);
//    vector<ModelTriangle> triangles;
//    vector<vec3> vertices;
//    vector<TexturePoint> texturePoints;
//
//    if(!file.is_open()){
//        cerr<<"Error: Could not open file "<<filename<<endl;
//        return triangles;;
//    }
//
//    string line;
//    vector<pair<string, Colour>> palette;
//    loadMtlFile("../textured-cornell-box.mtl",palette,textureFile);
//    string currentMaterial;
//
//    while(getline(file,line)){
//        if (line.empty()) {
//            continue;
//        }
//        std::vector<std::string> tokens = split(line, ' ');
//        if (tokens[0] == "v") {
//            vec3 vertex;
//            vertex.x= std::stof(tokens[1]);
//            vertex.y = std::stof(tokens[2]);
//            vertex.z = std::stof(tokens[3]);
//            vertex *= scalingFactor;
//            vertices.push_back(vertex);
//            //triangles.push_back(ModelTriangle(triangles.back(vertex.x),vertex.y,vertex.z,Colour{255,255,255}));
//        }
//        if (tokens[0] == "vt"){
//            TexturePoint t;
//            t.x = stof(tokens[1]);
//            t.y = stof(tokens[2]);
//            texturePoints.push_back(t);
//        } else if (tokens[0] == "f") {
//            vector<string> v1_tokens = split(tokens[1], '/');
//            vector<string> v2_tokens = split(tokens[2], '/');
//            vector<string> v3_tokens = split(tokens[3], '/');
//
//            int v1 = stoi(v1_tokens[0]) - 1;
//            int v2 = stoi(v2_tokens[0]) - 1;
//            int v3 = stoi(v3_tokens[0]) - 1;
//
//
//            Colour triangleColour; // Default colour
//            for (const auto& data : palette) {
//                if (data.first == currentMaterial) {
//                    triangleColour = data.second;
//                    cout << currentMaterial << endl;
//                    break;
//                }
//            }
//            ModelTriangle triangle(vertices[v1], vertices[v2], vertices[v3], triangleColour);
//                if (v1_tokens.size() > 1 && v2_tokens.size() > 1 && v3_tokens.size() > 1) {
//                int vt1 = stoi(v1_tokens[1]) - 1;
//                int vt2 = stoi(v2_tokens[1]) - 1;
//                int vt3 = stoi(v3_tokens[1]) - 1;
//
//                triangle.texturePoints[0] = texturePoints[vt1];
//                triangle.texturePoints[1] = texturePoints[vt2];
//                triangle.texturePoints[2] = texturePoints[vt3];
//           }
//            triangles.push_back(triangle);
//        } else if (tokens[0] == "usemtl") {
//            currentMaterial = tokens[1];
//        }
//    }
//    file.close();
//    return triangles;
//}

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
            //triangles.push_back(ModelTriangle(triangles.back(vertex.x),vertex.y,vertex.z,Colour{255,255,255}));
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

