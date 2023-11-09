#include "CameraPosition.h"


enum RenderMode {
    RENDER_NONE,
    RENDER_WIREFRAME,
    RENDER_RASTERISED,
    RENDER_TRACED,
};

RenderMode currentRenderMode = RENDER_NONE;
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

//mat3 lookAt(vec3 &cameraPosition) {
//    vec3 forward = normalize(cameraPosition - vec3(0,0,0));
//    vec3 right = normalize(cross(vec3(0,1,0), forward));
//    vec3 up = normalize(cross(forward, right));
//
//    return mat3(right,up,-forward);
//}

//void orbit(vec3 &cameraPosition){
//    vec3 target = vec3(0,0,0);
//    if (orbiting) {
//        vec3 relativePosition = cameraPosition - target;
//        vec3 newPosition;
//        newPosition.x = relativePosition.x * cos(M_PI / 180) - relativePosition.z * sin(M_PI / 180);
//        newPosition.y = relativePosition.y;
//        newPosition.z = relativePosition.x * sin(M_PI / 180) + relativePosition.z * cos(M_PI / 180);
//        cameraOrientation = lookAt(cameraPosition);
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

void Change_cameraPosition(vector<ModelTriangle> triangles,vec3 &cameraPosition, SDL_Event event, float focalLength, DrawingWindow &window) {
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
        else if (event.key.keysym.sym == SDLK_1) {
            currentRenderMode = RENDER_WIREFRAME;
        }
        else if (event.key.keysym.sym == SDLK_2) {
            currentRenderMode = RENDER_RASTERISED;
        }
        else if (event.key.keysym.sym == SDLK_3) {
            currentRenderMode = RENDER_TRACED;
        }
    }

    window.clearPixels();
    switch (currentRenderMode) {
        case RENDER_WIREFRAME:
            drawWireframe(triangles, cameraPosition, focalLength, window, cameraOrientation);
            break;
        case RENDER_RASTERISED:
            drawRasterisedModel(triangles, cameraPosition, focalLength, window, cameraOrientation);
            break;
        case RENDER_TRACED:
            drawRayTracedScene(triangles,window,cameraPosition,focalLength,cameraOrientation);
            break;
        case RENDER_NONE:
            break;
    }

}