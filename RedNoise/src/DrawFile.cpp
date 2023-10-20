#include <DrawingWindow.h>
#include "Interpolate.h"
#include "DrawFile.h"
#include "GetCanvasIntersectionPoint.h"


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
    for (float i=0.0; i<numberOfSteps; ++i) {
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

void drawlineWithDepth(CanvasPoint& from, CanvasPoint& to, Colour colour, DrawingWindow &window, std::vector<std::vector<float>>& depthBuffer) {
    float xDiff = to.x - from.x;
    float yDiff = to.y - from.y;
    float zDiff = to.depth - from.depth;
    float numberOfSteps = 5 * std::max(abs(xDiff), abs(yDiff));
    float xStepSize = xDiff/numberOfSteps;
    float yStepSize = yDiff/numberOfSteps;
    float zStepSize = zDiff/numberOfSteps;

    for (float i=0.0; i<numberOfSteps; ++i) {
        float x = from.x + (xStepSize * i);
        float y = from.y + (yStepSize * i);
        float z = from.depth + (zStepSize * i);

        int roundedX = round(x);
        int roundedY = round(y);

            if (z < depthBuffer[roundedY][roundedX]) {
                uint32_t ColourVal = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
                window.setPixelColour(roundedX, roundedY, ColourVal);
                depthBuffer[roundedY][roundedX] = z;
            }
    }
}

// Week3 Task 3
void drawTriangle(CanvasTriangle triangle,Colour input_colour, DrawingWindow &window){
    drawline(triangle.v0(),triangle.v1(),input_colour,window);
    drawline(triangle.v1(),triangle.v2(),input_colour,window);
    drawline(triangle.v2(),triangle.v0(),input_colour,window);
}

// Week3 Task 4 - second version
void drawFilledTriangle(CanvasTriangle triangle, Colour input_colour, Colour line_Colour,DrawingWindow &window) {
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
    for (float y = top.y; y < middle.y; ++y) {
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
    for (float y = middle.y; y <= bottom.y; y++) {
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
    drawTriangle(triangle,line_Colour,window);
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
            int texX = static_cast<int>(texX1 + (texX2 - texX1) * t);
            int texY = static_cast<int>(texY1 + (texY2 - texY1) * t);

            // Calculate the index to access the texture pixel
            if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
                int index = texY * texture.width + texX;
                window.setPixelColour(x, y, texture.pixels[index]);
            }
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
            int texX = static_cast<int>(texX1 + (texX2 - texX1) * t);
            int texY = static_cast<int>(texY1 + (texY2 - texY1) * t);

            // Calculate the index to access the texture pixel

            if (texX >= 0 && texX < texture.width && texY >= 0 && texY < texture.height) {
                int index = texY * texture.width + texX;
                window.setPixelColour(x, y, texture.pixels[index]);
            }
        }
        left_x += slope3;
        right_x += slope2;
    }
    drawTriangle(triangle,Colour{255,255,255},window);
}

void drawRenderTriangle(CanvasTriangle triangle, Colour input_colour, DrawingWindow &window,vector<std::vector<float>>& depthBuffer) {
    if (triangle.v0().y > triangle.v1().y) swap(triangle.v0(), triangle.v1());
    if (triangle.v0().y > triangle.v2().y) swap(triangle.v0(), triangle.v2());
    if (triangle.v1().y > triangle.v2().y) swap(triangle.v1(), triangle.v2());
    CanvasPoint top = triangle.v0();
    CanvasPoint middle = triangle.v1();
    CanvasPoint bottom = triangle.v2();
    float slope1 = (middle.x - top.x) / (middle.y - top.y);
    float slope2 = (bottom.x - top.x) / (bottom.y - top.y);
//    float depth_slope1 = (middle.depth - top.depth) / (middle.y - top.y);
//    float depth_slope2 = (bottom.depth - top.depth) / (bottom.y - top.y);
    float left_x = top.x;
    float right_x = top.x;
    float left_depth = top.depth;
    float right_depth = top.depth;

    for (float y = top.y; y < middle.y; ++y) {
        CanvasPoint from(left_x, y, left_depth);
        CanvasPoint to(right_x, y, right_depth);
        drawlineWithDepth(from, to, input_colour, window, depthBuffer);

        right_x += slope1;
        left_x += slope2;
    }

    right_x = middle.x;
    float slope3 = (bottom.x - middle.x) / (bottom.y - middle.y);
//    float depth_slope3 = (bottom.depth - middle.depth) / (bottom.y - middle.y);

    for (float y = middle.y; y <= bottom.y; y++) {
        CanvasPoint from(left_x, y, left_depth);
        CanvasPoint to(right_x, y, right_depth);
        drawlineWithDepth(from, to, input_colour, window, depthBuffer);
        right_x += slope3;
        left_x += slope2;
    }
}

void drawPoint(vector<ModelTriangle> triangles,vec3 cameraPosition,float focalLength,DrawingWindow &window){
    for (int i = 0; i < triangles.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
            vec3 vertexPosition = triangles[i].vertices[j];
            float scaling_factor = 186;
            vec2 canvasPoint = getCanvasIntersectionPoint(cameraPosition, vertexPosition,scaling_factor,focalLength,window.width,window.height);
            cout << "("<< canvasPoint.x << "," << canvasPoint.y << ")" << endl;
            // Flip the Y-axis
            Colour colour = Colour{255, 255, 255};
            uint32_t input_Colour = (255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue;
            window.setPixelColour(canvasPoint.x, canvasPoint.y, input_Colour);
        }
    }
}

void drawWireframe(ModelTriangle triangle, vec3 cameraPosition, float focalLength, DrawingWindow &window) {
    CanvasPoint p1;
    CanvasPoint p2;
    for (int i = 0; i < 3; ++i) {
        vec3 vertexPosition1 = triangle.vertices[i];
        vec3 vertexPosition2 = triangle.vertices[(i + 1) % 3]; // Connect to the next vertex, wrapping around to the first.
        float scaling_factor = 186;
        vec2 canvasPoint1 = getCanvasIntersectionPoint(cameraPosition, vertexPosition1, focalLength, scaling_factor, window.width, window.height);
        vec2 canvasPoint2 = getCanvasIntersectionPoint(cameraPosition, vertexPosition2, focalLength,scaling_factor, window.width, window.height);

        // Flip the Y-axis
        canvasPoint1.x = window.width - canvasPoint1.x;
        canvasPoint2.x = window.width - canvasPoint2.x;
        p1 = CanvasPoint{round(canvasPoint1.x),round(canvasPoint1.y)};
        p2 = CanvasPoint{round(canvasPoint2.x),round(canvasPoint2.y)};

        drawline(p1,p2,Colour{255,255,255}, window);
    }
}

void drawWireframeModel(vector<ModelTriangle> triangles, vec3 cameraPosition, float focalLength, DrawingWindow &window) {
    for (int i = 0; i < triangles.size(); ++i) {
        drawWireframe(triangles[i], cameraPosition, focalLength, window);
    }
}

void drawTriangleModel(ModelTriangle triangle, vec3 cameraPosition, float focalLength, DrawingWindow &window, std::vector<std::vector<float>>& depthBuffer) {
    CanvasTriangle canvasTriangle;
    for (int i = 0; i < 3; ++i) {
        vec3 vertexPosition = triangle.vertices[i];
        float scaling_factor = 185;
        vec2 canvasPoint = getCanvasIntersectionPoint(cameraPosition, vertexPosition, focalLength, scaling_factor, window.width, window.height);
        CanvasPoint p = CanvasPoint{canvasPoint.x, canvasPoint.y};
        p.depth = calculateDepth(cameraPosition, vertexPosition);
        canvasTriangle.vertices[i] = p;
    }
    Colour inputColour = triangle.colour;
    drawRenderTriangle(canvasTriangle, inputColour, window, depthBuffer);
}

void drawFilledModel(vector<ModelTriangle> triangles, vec3 cameraPosition, float focalLength, DrawingWindow &window) {
    int width = window.width;
    int height = window.height;
    std::vector<std::vector<float>> depthBuffer(height, std::vector<float>(width, std::numeric_limits<float>::max()));

    for (int i = 0; i < triangles.size(); ++i) {
        drawTriangleModel(triangles[i], cameraPosition, focalLength, window, depthBuffer);
    }
}
