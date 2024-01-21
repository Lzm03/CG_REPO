# COMS30020: Computer Graphics

Welcome to the Computer Graphics course for 2023!

## Course Overview
This course （<a href="https://github.com/COMS30020/CG2023">Weekly Workbooks</a>） aims to equip students with the fundamental concepts and skills in computer graphics through a series of progressive weekly assignments. Here's an overview of each week's focus:

### Week 1: RedNoise
- **Topic**: Template project to test `SDL`.
- **How to Run**: Use the `make` command.

### Week 2: Interpolate
- **Topic**: Demonstrating pixel display and drawing, including grayscale interpolation and two-dimensional color interpolation.
- **How to Run**: Use the `make` command.

### Week 3: Triangles
- **Topic**: Demonstrating line drawing, triangle drawing, filling, and texture mapping.
- **How to Run**: Use the `make` command, with `u`, `f` keys for different effects.

### Week 4: Wireframe
- **Topic**: Demonstrating loading of `.obj` and `.mtl` files, projecting onto a 2D image plane using rasterization.
- **How to Run**: Use the `make` command.

### Week 5: Camera
- **Topic**: Demonstrating navigation and transformation of a 3D space projected onto a 2D plane.
- **How to Run**: Use the `make` command, with keyboard for camera operation.

### Week 6: Ray
- **Topic**: Demonstrating the ray tracing method of projecting 3D points onto a 2D image plane, including shadow effects.
- **How to Run**: Use `make` or `make speedy` command, with keyboard operation.

### Week 7: Light
- **Topic**: Demonstrating various lighting effects on a ray-traced projection of 3D points.
- **How to Run**: Use `make` or `make speedy` command, with keyboard for light control.

## Coursework
- **Modelling**: Various 3D `.obj` files are included, ranging from simple to increasingly complex environments.
- **Rendering**: Three rendering options available: wireframe, rasterize, and raytrace.
- **Lighting**: Includes proximity lighting, angle of incidence lighting, specular lighting, and ambient lighting.
- **materials**: Includes mirror and metal.

## Usage Instructions
- **Build and Run**: Use the included `Makefile`.
- **Animation**: Uncomment the following line inside `main()` to enable renderFrame.
- **Keyboard Controls**: A detailed guide to keyboard operations.
