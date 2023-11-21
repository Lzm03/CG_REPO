#include "DrawFile.h"
#include "Interpolate.h"
#include "LoadFile.h"



#define WIDTH 320 * 4
#define HEIGHT 240 * 4

using namespace std;
using namespace glm;

enum RenderMode {
    RENDER_NONE,
    RENDER_WIREFRAME,
    RENDER_RASTERISED,
    RENDER_TRACED,
    RENDER_GOURAUD,
};
RenderMode currentRenderMode = RENDER_NONE;

bool orbiting = false;
mat3 cameraOrientation(
        vec3(-1.0,0.0,0.0),
        vec3(0.0, 1.0,0.0),
        vec3(0.0,0.0,1.0)
);

vec3 Boxlight(0.0,0.4f,0.3f);
vec3 Spherelight(0.0f,1.0f,1.0f);

vec3 cameraPosition(0.0f, 0.0f, 4.0f);


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
        cameraPosition = cameraPosition * Rotate_Y(-M_PI/180);
        cameraOrientation = cameraOrientation * Rotate_Y(-M_PI/180);
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

vector<ModelTriangle> combineAllTriangles(const unordered_map<string, vector<ModelTriangle>>& allTriangles) {
    vector<ModelTriangle> combinedTriangles;
    for (const auto& fileTriangles : allTriangles) {
        combinedTriangles.insert(combinedTriangles.end(), fileTriangles.second.begin(), fileTriangles.second.end());
    }
    return combinedTriangles;
}

void Change_cameraPosition(SDL_Event event, DrawingWindow &window) {
    float translationAmount = 0.1f;
    float scalingFactor = 0.35f;
    float focalLength = 2.0f;
//    vector<ModelTriangle> triangles_1 = loadObjFile("../cornell-box.obj", scalingFactor, loadMtlFile("../cornell-box.mtl"));
//    vector<ModelTriangle> triangles_2 = loadObjFile("../sphere.obj", scalingFactor, loadMtlFile("../cornell-box.mtl"));

    vector<string> filenames = {"../textured-cornell-box.obj", "../sphere.obj","../logo.obj"};
    unordered_map<string, vector<ModelTriangle>> allTriangles = loadMultipleObjFiles(filenames, scalingFactor, loadMtlFile("../textured-cornell-box.mtl"));
    vector<ModelTriangle> box = allTriangles["../textured-cornell-box.obj"];
    vector<ModelTriangle> sphere = allTriangles["../sphere.obj"];
    vector<ModelTriangle> logo = allTriangles["../logo.obj"];

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
        if (event.key.keysym.sym == SDLK_x) Boxlight.x += translationAmount;
        else if (event.key.keysym.sym == SDLK_z) Boxlight.x -= translationAmount;
        else if (event.key.keysym.sym == SDLK_b) Boxlight.y -= translationAmount;
        else if (event.key.keysym.sym == SDLK_n) Boxlight.y += translationAmount;
        else if (event.key.keysym.sym == SDLK_c) Boxlight.z += translationAmount;
        else if (event.key.keysym.sym == SDLK_v) Boxlight.z -= translationAmount;
        else if (event.key.keysym.sym == SDLK_h) look_At();
        else if (event.key.keysym.sym == SDLK_o) orbiting = !orbiting;
        else if (event.key.keysym.sym == SDLK_r) ResetCamera();
        else if (event.key.keysym.sym == SDLK_1) {
            currentRenderMode = RENDER_WIREFRAME;
        }
        else if (event.key.keysym.sym == SDLK_2) {
            currentRenderMode = RENDER_RASTERISED;
        }
        else if (event.key.keysym.sym == SDLK_3) {
            currentRenderMode = RENDER_TRACED;
        }
        else if (event.key.keysym.sym == SDLK_4) {
            currentRenderMode = RENDER_GOURAUD;
        }
    }
    window.clearPixels();
    switch (currentRenderMode) {
        case RENDER_WIREFRAME:
            drawWireframe(box, cameraPosition, focalLength, window, cameraOrientation,320.0f);
            break;
        case RENDER_RASTERISED:
            drawRasterisedModel(box, cameraPosition, focalLength, window, cameraOrientation,320.0f);
//            drawRasterisedModel(logo, cameraPosition, focalLength, window, cameraOrientation,5.0f);
            break;
        case RENDER_TRACED:
            drawRayTracedScene(box,window,cameraPosition,focalLength,cameraOrientation,Boxlight,180.0f);
            break;
        case RENDER_GOURAUD:
            drawPhong(calculateVertexNormals(sphere),window,cameraPosition,focalLength,cameraOrientation,Spherelight, 250.0f);
//            drawGouraud(calculateVertexNormals(sphere),window,cameraPosition,focalLength,cameraOrientation,Spherelight,250.0f);
            break;
        case RENDER_NONE:
            break;
    }

}

void renderFrame(vector<ModelTriangle> &triangles, DrawingWindow &window, float focalLength) {
    const int frameCount = 48;
    const int n_zero = 5;
    const float cameraMoveStep = 0.075f;

//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        drawWireframe(triangles, cameraPosition, focalLength, window, cameraOrientation, 500.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//    }

//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        drawRasterisedModel(triangles, cameraPosition, focalLength, window, cameraOrientation, 500.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output1/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//    }

//    for (int frame = 0; frame < frameCount; ++frame) {
//        draw(window);
//        drawRayTracedScene(triangles,window,cameraPosition,focalLength,cameraOrientation,Boxlight,300.0f);
//        string frameNumber = string(n_zero - to_string(frame).length(), '0') + to_string(frame);
//        window.savePPM("../output2/" + frameNumber + ".ppm");
//        cout << "saved " << frame << endl;
//    }

}


    int main(int argc, char *argv[]) {
        DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
        SDL_Event event;

//  SingleFloats
        std::vector<float> result;
        result = interpolateSingleFloats(2.2, 8.5, 7);
        for (size_t i = 0; i < result.size(); i++) {
            cout << result[i] << " "; // Format into text string
        }
        cout << endl;

//  ThreeElementValues
        vector<vec3> result2;
        result2 = interpolateThreeElementValues(vec3(1.0, 4.0, 9.2), vec3(4.0, 1.0, 9.8), 4);
        for (size_t i = 0; i < result2.size(); i++) {
            vec3 currentVec = result2[i];
            cout << "(" << currentVec.x << ", " << currentVec.y << ", " << currentVec.z << ") ";
            cout << endl;
        }

        vector<string> filenames = {"../cornell-box.obj", "../sphere.obj"};
        unordered_map<string, vector<ModelTriangle>> allTriangles = loadMultipleObjFiles(filenames, 0.35f, loadMtlFile("../cornell-box.mtl"));
        vector<ModelTriangle> box = allTriangles["../cornell-box.obj"];
        vector<ModelTriangle> sphere = allTriangles["../sphere.obj"];

        renderFrame(box,window,2.0f);



//  drawColour(window);
        while (true) {
//        drawLineFromTopRightToCenter(window);
//        drawLineFromTopLeftToCenter(window);
//        drawHorizontalLine(window);
//        drawVerticalLine(window);
            //implementation of Draw Triangle
            CanvasTriangle input_triangle = CanvasTriangle{CanvasPoint(rand() % 320, rand() % 240),
                                                           CanvasPoint(rand() % 320, rand() % 240),
                                                           CanvasPoint(rand() % 320, rand() % 240)};
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_u)
                    drawTriangle(input_triangle, Colour{rand() % 256, rand() % 256, rand() % 256}, window);
                else if (event.key.keysym.sym == SDLK_f)
                    drawFilledTriangle(input_triangle, Colour{rand() % 256, rand() % 256, rand() % 256},
                                       Colour{255, 255, 255}, window);
            }
            Change_cameraPosition(event, window);
            orbit();
//        CanvasPoint v0(160, 10);
//        CanvasPoint v1(300, 230);
//        CanvasPoint v2(10, 150);
//        v0.texturePoint = TexturePoint{195, 5};
//        v1.texturePoint = TexturePoint{395, 380};
//        v2.texturePoint = TexturePoint{65, 330};
//        CanvasTriangle triangle(v0, v1, v2);
//        TextureMap texture = TextureMap("../logo_texture.ppm");
//        drawTexturedTriangle(triangle,texture,window);

            // We MUST poll for events - otherwise the window will freeze !
            if (window.pollForInputEvents(event)) handleEvent(event, window);
            // Need to render the frame at the end, or nothing actually gets shown on the screen !
            window.renderFrame();

        }
    }
