#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "CanvasTriangle.h"
using namespace std;
using namespace glm;


// Week2 Task 2
vector<float> interpolateSingleFloats(float from, float to, int numberOfValues){
    vector<float> v;
    float x;
    x = (to - from) / (numberOfValues - 1);
    for (int i = 0; i < numberOfValues; ++i){
        v.push_back(from);
        from = from + x;
    }
    return v;
}

// Week2 Task 4
vector<vec3> interpolateThreeElementValues(vec3 from,vec3 to, int numberOfValues){
    vec3 v = to - from;
    vec3 result;
    result.x = v.x / (numberOfValues - 1); // 1.0
    result.y = v.y / (numberOfValues - 1); // -1.0
    result.z = v.z / (numberOfValues - 1); // 0.2

    vector<vec3> results;
    for (int i = 0; i < numberOfValues; ++i) {
        results.push_back(from);
        from = from + result;
    }
    return results;
}

vector<CanvasPoint> interpolateTwoElementValues(CanvasPoint from,CanvasPoint to, int numberOfValues){
    vector<CanvasPoint> points;
    float x_step = (to.x - from.x)/(numberOfValues-1);
    float y_step = (to.y - from.y)/(numberOfValues-1);
    float d_step = (to.depth - from.depth)/(numberOfValues-1);

    CanvasPoint temp = from;
    points.push_back(temp);

    for (int i = 0; i < numberOfValues-1; i++) {
        temp.x = temp.x + x_step;
        temp.y = temp.y + y_step;
        temp.depth = temp.depth + d_step;
        points.push_back(temp);
    }
    return points;
}

//vector<TexturePoint> interpolateTwoElementValues(TexturePoint from, TexturePoint to, int numberOfValuess){
//    vector<TexturePoint> points;
//    float x_step = (to.x - from.x)/(numberOfValues-1);
//    float y_step = (to.y - from.y)/(numberOfValues-1);
//
//    TexturePoint temp = from;
//    points.push_back(temp);
//
//    for (int i = 0; i < numberOfValues-1; i++) {
//        temp.x = temp.x + x_step;
//        temp.y = temp.y + y_step;
//        points.push_back(temp);
//    }
//    return points;
//}
