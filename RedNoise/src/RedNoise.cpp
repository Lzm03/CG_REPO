#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#define WIDTH 320
#define HEIGHT 240

using namespace std;
using namespace glm;

void draw(DrawingWindow &window) {
	window.clearPixels();

    // Task 3
	for (size_t y = 0; y < window.height; y++) {
		for (size_t x = 0; x < window.width; x++) {
            float red = (1 - float(x) / float(window.width - 1)) * 255;
			float green = (1 - float(x) / float(window.width - 1)) * 255;
			float blue = (1 - float(x) / float(window.width - 1)) * 255;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
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

// Task 2
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

// Task 4
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

// Task 5
void drawColour(DrawingWindow &window){
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow
    vector<vec3> results;
    vector<vec3> Left = interpolateThreeElementValues(topLeft,bottomLeft,window.height);
    vector<vec3> Right = interpolateThreeElementValues(topRight,bottomRight,window.height);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            vec3 left = Left[y];
            vec3 right = Right[y];
            vector<vec3> row = interpolateThreeElementValues(left,right,window.width);
            vec3 currentColour = row[x];
            uint32_t colour = (255 << 24) + (int(currentColour.x) << 16) + (int(currentColour.y) << 8) + int(currentColour.z);
            window.setPixelColour(x, y, colour);
        }
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

    while (true) {
        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        drawColour(window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();
    }
}
