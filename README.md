# GLSL 3D Graphics Project

This repository contains a 3D graphics application built with OpenGL and GLSL (OpenGL Shading Language). The project implements a 3D rendering system with lighting, texturing, camera movement, and model loading capabilities.

## Project Structure

### Root Directory

- **main.cpp**: The entry point of the application. Sets up the OpenGL window, initializes necessary objects, and contains the main rendering loop.
- **object.h**: Defines the `Object` class for loading and rendering simple 3D objects from OBJ files.
- **CMakeLists.txt**: The CMake configuration file for building the project.
- **Makefile**: Generated build configuration.

### Shader Files

- **default.vert**: The vertex shader for regular 3D objects. Handles vertex transformations and passes lighting data to the fragment shader.
- **default.frag**: The fragment shader for regular objects. Implements three lighting models (point light, directional light, and spotlight) with ambient, diffuse, and specular components.
- **light.vert**: A simplified vertex shader for light source objects.
- **light.frag**: A simplified fragment shader for light source objects.

### Source Directory (`src/`)

The `src/` directory contains the core engine components:

- **Camera System**:
  - `Camera.h`, `Camera.cpp`: Implements a movable 3D camera with keyboard/mouse controls.

- **Shader Management**:
  - `shaderClass.h`, `shaderClass.cpp`: Handles loading, compiling, and using GLSL shaders.

- **Model Loading**:
  - `model.h`, `model.cpp`: Manages loading of complex 3D models from OBJ files.
  - `Mesh.h`, `Mesh.cpp`: Represents a 3D mesh with vertices, indices, and textures.

- **OpenGL Abstractions**:
  - `VAO.h`, `VAO.cpp`: Vertex Array Object management.
  - `VBO.h`, `VBO.cpp`: Vertex Buffer Object management.
  - `EBO.h`, `EBO.cpp`: Element Buffer Object management.
  - `Texture.h`, `Texture.cpp`: Texture loading and management.

### Assets Directory (`assets/`)

- **objects/**: Contains 3D models in OBJ format:
  - Various sphere models (smooth, coarse)
  - A bunny model (different sizes)
  - A desert city model
  - Cube and plane primitives

- **textures/**: Contains texture files for 3D objects:
  - Diffuse textures (color/albedo maps)
  - Normal maps
  - Specular maps

## Core Functionality

1. **3D Rendering**: The project can render 3D models with materials and textures.
2. **Lighting System**: Implements three lighting models:
   - Point light: Light emits in all directions with distance attenuation
   - Directional light: Light comes from a specific direction with no attenuation
   - Spotlight: Light emits in a cone with inner and outer boundaries
3. **Camera Movement**: First-person style camera controlled with keyboard and mouse.
4. **Day/Night Cycle**: Animates the sun position to create a day/night cycle with changing light color.
5. **Model Loading**: Supports loading 3D models from OBJ files.
6. **Texture Mapping**: Applies diffuse and specular maps to 3D models.

## How to Modify the Project

### Modifying Textures

To change textures applied to 3D models:

1. Add your texture files to the `assets/textures/` directory.
2. Modify the texture loading code in `main.cpp`:

```cpp
// Texture data
Texture textures[]
{
    Texture((texPath + "your_texture.png").c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
    Texture((texPath + "your_specular.png").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
};
```

### Changing 3D Models

To load different 3D models:

1. Add your OBJ file to the `assets/objects/` directory.
2. Modify the model loading code in `main.cpp`:

```cpp
// Change the path to your model file
Model model("assets/objects/your_model.obj");
```

### Modifying Lighting

To adjust lighting parameters:

1. **Change Light Position**: Modify the `lightPos` vector in `main.cpp`.
2. **Change Light Color**: Modify the `lightColor` vector in `main.cpp`.
3. **Adjust Light Intensity**: In `default.frag`, adjust the attenuation parameters (`a` and `b`) in the `pointLight()` function.
4. **Change Light Type**: In `default.frag`, modify the `main()` function to call a different light function (pointLight, direcLight, or spotLight).

### Adjusting the Camera

To modify camera behavior:

1. Open `src/Camera.h`.
2. Adjust the `speed` and `sensitivity` values to change movement speed and mouse sensitivity.
3. Modify the starting position in `main.cpp` when creating the camera:

```cpp
Camera camera(width, height, glm::vec3(x, y, z)); // Change x, y, z to your desired values
```

### Creating Custom Shaders

To create custom shader effects:

1. Create new shader files (e.g., `custom.vert` and `custom.frag`) in the root directory.
2. Initialize the shader in `main.cpp`:

```cpp
Shader customShader("custom.vert", "custom.frag");
```
3. Apply the shader to your models:

```cpp
model.Draw(customShader, camera);
```

### Modifying Animations

The day/night cycle animation can be adjusted in `main.cpp`:

1. Change `sunRadius` to adjust the circular orbit radius.
2. Change `sunHeight` to adjust the height of the sun's path.
3. Change `sunSpeed` to adjust how fast the sun moves.

## Building and Running

The project uses CMake for building:

1. Make sure you have OpenGL, GLFW, and GLM installed.
2. Run `cmake .` to generate the build files.
3. Run `make` to build the project.
4. Execute the resulting binary to run the application.

## Controls

- **W, A, S, D**: Move the camera forward, left, backward, and right.
- **Space**: Move the camera up.
- **Left Control**: Move the camera down.
- **Mouse**: Look around.
- **Escape**: Close the application. 