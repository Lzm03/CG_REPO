#include "LoadFile.h"



vector<ModelTriangle> loadObjFile(const std::string& filename, float scalingFactor){
    ifstream file(filename);
    vector<ModelTriangle> triangles;
    vector<vec3> vertices;

    if(!file.is_open()){
        cerr<<"Error: Could not open file "<<filename<<endl;
        return triangles;;
    }

    string line;
    vector<pair<string, Colour>> palette;
    loadMtlFile("../cornell-box.mtl",palette);
    string currentMaterial;

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
        if (tokens[0] == "f") {
            vector<string> parts = split(line, '/');
            int v1, v2, v3;
            v1 = std::stoi(tokens[1]) - 1;
            v2 = std::stoi(tokens[2]) - 1;
            v3 = std::stoi(tokens[3]) - 1;

            Colour triangleColour; // Default colour
            for (const auto& data : palette) {
                if (data.first == currentMaterial) {
                    triangleColour = data.second;
                    cout<<currentMaterial<<endl;
                    break;
                }
            }
            cout<<triangleColour<<endl;
            ModelTriangle triangle(vertices[v1], vertices[v2], vertices[v3], triangleColour);
            triangles.push_back(triangle);
        }
        if(tokens[0] == "usemtl"){
            currentMaterial = tokens[1];
        }
    }

    for (const ModelTriangle& triangle : triangles) {
        std::cout << triangle << std::endl;
    }

    file.close();
    return triangles;
}

void loadMtlFile(const std::string& filename,vector<pair<string, Colour>>& palette) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open material file " << filename << std::endl;
        return;
    }
    string line;
    string material;
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
            palette.emplace_back(material, Colour(red, green, blue));
        }
    }
    file.close();
}
