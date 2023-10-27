#include "CameraPosition.h"

bool orbiting = false;
mat3 cameraOrientation(
        vec3(1.0,0.0,0.0),
        vec3(0.0, 1.0,0.0),
        vec3(0.0,0.0,1.0)
);


mat3 Rotate_X(float t){
    return mat3(
            vec3( 1.0,    0.0,    0.0),
            vec3( 0.0, cos(t),-sin(t)),
            vec3( 0.0, sin(t), cos(t))
            );
}

mat3 Rotate_Y(float t){
    return mat3(
            vec3( cos(t), 0.0, sin(t)),
            vec3(0.0, 1.0,    0.0),
            vec3(-sin(t), 0.0, cos(t))
            );
}

//void lookAt(vec3 &cameraPosition) {
//    vec3 forward = normalize(cameraPosition - vec3(0,0,0));
//    vec3 right = normalize(cross(vec3(0,1,0), forward));
//    vec3 up = normalize(cross(forward, right));
//
//    cameraOrientation[0] = right;
//    cameraOrientation[1] = up;
//    cameraOrientation[2] = forward;
//}

//void orbit(vec3 &cameraPosition){
//    if (orbiting) {
//        cameraPosition = cameraPosition * Rotate_Y(-M_PI / 180);
//        cameraOrientation = cameraOrientation * Rotate_Y(-M_PI / 180);
//        lookAt(cameraPosition);
//    }
//}

void orbit(vec3 &cameraPosition) {
    vec3 target = vec3(0,0,0);
    if (orbiting) {
        cameraPosition = (cameraPosition - target) * Rotate_Y(-M_PI / 180) + target;
        cameraOrientation = cameraOrientation * Rotate_Y(-M_PI / 180);
    }
}

void ResetCamera(vec3 &cameraPosition) {
    cameraPosition = vec3(0.0,0.0,4.0);
    cameraOrientation = mat3(
            vec3(1.0,0.0,0.0),
            vec3(0.0,1.0,0.0),
            vec3(0.0,0.0,1.0));
}

void Change_cameraPosition(vector<ModelTriangle> triangles, vec3 &cameraPosition, SDL_Event event, float focalLength, DrawingWindow &window) {
    float translationAmount = 0.1f;
    if (event.type == SDL_KEYDOWN) {
        // Camera translation
        if (event.key.keysym.sym == SDLK_a) cameraPosition.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_d) cameraPosition.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_w) cameraPosition.y -= translationAmount;
        else if (event.key.keysym.sym == SDLK_s) cameraPosition.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_q) cameraPosition.z += translationAmount;
        else if (event.key.keysym.sym == SDLK_e) cameraPosition.z -= translationAmount;
        // Camera rotation
        if (event.key.keysym.sym == SDLK_UP) {
            cameraPosition = cameraPosition * Rotate_X(M_PI / 180);
        }
        else if (event.key.keysym.sym == SDLK_DOWN) {
            cameraPosition = cameraPosition * Rotate_X(-M_PI/180);
        }
        else if (event.key.keysym.sym == SDLK_LEFT) {
            cameraPosition = cameraPosition * Rotate_Y(M_PI/180);
        }
        else if (event.key.keysym.sym == SDLK_RIGHT) {
            cameraPosition = cameraPosition * Rotate_Y(-M_PI/180);
        }
        // Camera orientation rotation
        if (event.key.keysym.sym == SDLK_l) {
            cameraOrientation = cameraOrientation * Rotate_Y(-M_PI / 180);
        }
        else if (event.key.keysym.sym == SDLK_j) {
            cameraOrientation = cameraOrientation * Rotate_Y(M_PI / 180);
        }
        else if (event.key.keysym.sym == SDLK_k) {
            cameraOrientation = cameraOrientation * Rotate_X(-M_PI/180);
        }
        else if (event.key.keysym.sym == SDLK_i)  {
            cameraOrientation = cameraOrientation * Rotate_X(M_PI / 180);
        }
//        else if (event.key.keysym.sym == SDLK_h) lookAt(cameraPosition);
        else if (event.key.keysym.sym == SDLK_o) orbiting = !orbiting;
        else if (event.key.keysym.sym == SDLK_r) ResetCamera(cameraPosition);
    }

    window.clearPixels();
    drawRenderModel(triangles ,cameraPosition, focalLength, window,cameraOrientation);
//  drawWireframeModel(triangles,cameraPosition,focalLength,window,cameraOrientation);
}