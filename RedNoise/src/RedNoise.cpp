#include "DrawFile.h"
#include "Interpolate.h"
#include "LoadFile.h"

#define WIDTH 320
#define HEIGHT 240

using namespace std;
using namespace glm;

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

int main(int argc, char *argv[]) {
    DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
    SDL_Event event;

//  SingleFloats
    std::vector<float> result;
    result = interpolateSingleFloats(2.2, 8.5, 7);
    for(size_t i=0; i<result.size(); i++){
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

    string objFileName = "../cornell-box.obj";
    float scalingFactor = 0.35;
    vector<ModelTriangle> triangles = loadObjFile(objFileName, scalingFactor);


    vec3 cameraPosition = vec3{0.0f, 0.0f, 4.0f};
    float focalLength = 2.0f;
//    drawPoint(triangles,cameraPosition,focalLength,window);
//    drawWireframeModel(triangles,cameraPosition,focalLength,window);
    drawFilledModel(triangles,cameraPosition,focalLength,window);

//  drawColour(window);
    while (true) {
//        drawLineFromTopRightToCenter(window);
//        drawLineFromTopLeftToCenter(window);
//        drawHorizontalLine(window);
//        drawVerticalLine(window);
        //implementation of Draw Triangle
        CanvasTriangle input_triangle = CanvasTriangle{CanvasPoint(rand()%320,rand()%240),CanvasPoint(rand()%320,rand()%240),CanvasPoint(rand()%320,rand()%240)};
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_u) drawTriangle(input_triangle, Colour{rand()%256,rand()%256,rand()%256}, window);
            else if(event.key.keysym.sym == SDLK_f) drawFilledTriangle(input_triangle, Colour{rand()%256,rand()%256,rand()%256},Colour{255,255,255},window);
        }

//        TextureMap texture = TextureMap("texture.ppm");
//        CanvasPoint v0(160, 10);
//        CanvasPoint v1(300, 230);
//        CanvasPoint v2(10, 150);
//        v0.texturePoint = TexturePoint{195, 5};
//        v1.texturePoint = TexturePoint{395, 380};
//        v2.texturePoint = TexturePoint{65, 330};
//        CanvasTriangle triangle(v0, v1, v2);
//        drawTexturedTriangle(triangle,texture,window);

        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();

    }
}

