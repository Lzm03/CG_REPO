#include "Interpolate.h"


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
