#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <CanvasTriangle.h>
#include <TextureMap.h>
#include <algorithm>



#define WIDTH 320
#define HEIGHT 240

using namespace std;
using namespace glm;

void draw(DrawingWindow &window) {
	window.clearPixels();

    // Week2 Task 3
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

// Week2 Task 5
void drawColour(DrawingWindow &window){
    window.clearPixels();
    glm::vec3 topLeft(255, 0, 0);        // red
    glm::vec3 topRight(0, 0, 255);       // blue
    glm::vec3 bottomRight(0, 255, 0);    // green
    glm::vec3 bottomLeft(255, 255, 0);   // yellow
    vec3 results;
    vector<vec3> Left = interpolateThreeElementValues(topLeft,bottomLeft,window.height);
    vector<vec3> Right = interpolateThreeElementValues(topRight,bottomRight,window.height);
    for (size_t y = 0; y < window.height; y++) {
        for (size_t x = 0; x < window.width; x++) {
            vec3 left = Left[y];
            vec3 right = Right[y];
            vector<vec3> row = interpolateThreeElementValues(left,right,window.width);
            results = row[x];
            uint32_t colour = (255 << 24) + (int(results.x) << 16) + (int(results.y) << 8) + int(results.z);
            window.setPixelColour(x, y, colour);
        }
    }
}

// Week3 Task 2
void drawline(CanvasPoint& from, CanvasPoint& to, Colour colour, DrawingWindow &window){
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float numberOfSteps = std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff/numberOfSteps;
    float yStepSize = yDiff/numberOfSteps;
    for (float i=0; i<numberOfSteps; ++i) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        uint32_t Colour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
        window.setPixelColour(round(x), round(y), Colour);
    }
}

void drawLineFromTopLeftToCenter(DrawingWindow &window) {
    float x1 = 0;
    float y1 = 0;
    float x2 = window.width / 2;
    float y2 = window.height / 2;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

void drawLineFromTopRightToCenter(DrawingWindow &window) {
    float x1 = window.width - 1;
    float y1 = 0;
    float x2 = window.width / 2;
    float y2 = window.height / 2;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

void drawVerticalLine(DrawingWindow &window) {
    float x1 = window.width / 2;
    float y1 = 0;
    float x2 = window.width / 2;
    float y2 = window.height-1;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

void drawHorizontalLine(DrawingWindow &window) {
    float lineWidth = window.width / 3;
    float x1 = (window.width - lineWidth) / 2;
    float x2 = x1 + lineWidth;
    float y1 = window.height / 2;
    float y2 = y1;
    CanvasPoint from = CanvasPoint{x1,y1};
    CanvasPoint to = CanvasPoint{x2,y2};
    drawline(from,to,Colour{255,255,255},window);
}

// Week3 Task 3
void drawTriangle(CanvasTriangle triangle,Colour input_colour, DrawingWindow &window){
    drawline(triangle.v0(),triangle.v1(),input_colour,window);
    drawline(triangle.v1(),triangle.v2(),input_colour,window);
    drawline(triangle.v2(),triangle.v0(),input_colour,window);
}

// Week3 Task 4 - second version
void drawFilledTriangle(CanvasTriangle triangle, Colour input_colour, Colour line_colour, DrawingWindow &window) {
    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());

    CanvasPoint top = triangle.v0();
    CanvasPoint middle = triangle.v1();
    CanvasPoint bottom = triangle.v2();

    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);

    float left_x = top.x;
    float right_x = top.x;

    // first part of triangle
    for (int y = top.y; y <= middle.y; ++y) {
        // start from top point
        int startX = round(right_x);
        int endX = round(left_x);
        CanvasPoint from(startX,y);
        CanvasPoint to(endX,y);
        drawline(from,to,input_colour,window);
        right_x += slope1;
        left_x += slope2;
    }

    // The slope of the third side
    right_x = middle.x;
    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);

    // second part of triangle
    for (int y = middle.y + 1; y <= bottom.y; y++) {
        // start from middle point
        int startX = round(right_x);
        int endX = round(left_x);
        CanvasPoint from(startX,y);
        CanvasPoint to(endX,y);
        drawline(from,to,input_colour,window);
        right_x += slope3;
        left_x += slope2;
    }

    // draw three white line of whole triangle
    drawTriangle(triangle,line_colour,window);
}

// Week3 Task 5
void drawTexturedTriangle(CanvasTriangle triangle, TextureMap texture, DrawingWindow &window) {
    // Sort vertices by y-coordinate
    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());

    CanvasPoint top = triangle.v0();
    CanvasPoint middle = triangle.v1();
    CanvasPoint bottom = triangle.v2();

    // Calculate slopes for the top and bottom edges
    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);

    float left_x = top.x;
    float right_x = top.x;

    // First part of triangle (top to middle)
    for (int y = top.y; y <= middle.y; ++y) {
        // Calculate the corresponding texturePoints for CanvasPoints
        float t1 = (y - top.y) / (middle.y - top.y);
        float t2 = (y - top.y) / (bottom.y - top.y);
        float texX1 = top.texturePoint.x + (middle.texturePoint.x - top.texturePoint.x) * t1;
        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
        float texY1 = top.texturePoint.y + (middle.texturePoint.y - top.texturePoint.y) * t1;
        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;

        // Draw the horizontal line with texture mapping
        for (int x = round(left_x); x <= round(right_x); ++x) {
            // Interpolate t based on the current x position
            float t = (x - left_x) / (right_x - left_x);
            int texX = texX1 + (texX2 - texX1) * t;
            int texY = texY1 + (texY2 - texY1) * t;

            // Calculate the index to access the texture pixel
            int index = texY * texture.width + texX;
            window.setPixelColour(x, y, texture.pixels[index]);
        }

        left_x += slope1;
        right_x += slope2;
    }

    // Initialize left and right x-coordinates for the bottom edge
    left_x = middle.x;
    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);

    // Second part of triangle (middle to bottom)
    for (int y = middle.y + 1; y <= bottom.y; ++y) {
        // Calculate the corresponding texture coordinates for Canvas Points
        float t1 = (y - middle.y) / (bottom.y - middle.y);
        float t2 = (y - top.y) / (bottom.y - top.y);
        float texX1 = middle.texturePoint.x + (bottom.texturePoint.x - middle.texturePoint.x) * t1;
        float texX2 = top.texturePoint.x + (bottom.texturePoint.x - top.texturePoint.x) * t2;
        float texY1 = middle.texturePoint.y + (bottom.texturePoint.y - middle.texturePoint.y) * t1;
        float texY2 = top.texturePoint.y + (bottom.texturePoint.y - top.texturePoint.y) * t2;

        // Draw the horizontal line with texture mapping
        for (int x = round(left_x); x <= round(right_x); ++x) {
            // Interpolate t based on the current x position
            float t = (x - left_x) / (right_x - left_x);
            int texX = texX1 + (texX2 - texX1) * t;
            int texY = texY1 + (texY2 - texY1) * t;

            // Calculate the index to access the texture pixel
            int index = texY * texture.width + texX;
            window.setPixelColour(x, y, texture.pixels[index]);
        }

        left_x += slope3;
        right_x += slope2;
    }

    drawTriangle(triangle,Colour{255,255,255},window);
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
            else if(event.key.keysym.sym == SDLK_f) drawFilledTriangle(input_triangle, Colour{rand()%256,rand()%256,rand()%256},Colour{255,255,255}, window);
        }

        TextureMap texture = TextureMap("texture.ppm");
        CanvasPoint v0(160, 10);
        CanvasPoint v1(300, 230);
        CanvasPoint v2(10, 150);
        v0.texturePoint = TexturePoint{195, 5};
        v1.texturePoint = TexturePoint{395, 380};
        v2.texturePoint = TexturePoint{65, 330};
        CanvasTriangle triangle(v0, v1, v2);
        drawTexturedTriangle(triangle,texture,window);

        // We MUST poll for events - otherwise the window will freeze !
        if (window.pollForInputEvents(event)) handleEvent(event, window);
        // Need to render the frame at the end, or nothing actually gets shown on the screen !
        window.renderFrame();

    }
}
