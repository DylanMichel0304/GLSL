# GLSL 3D Graphics Engine

This project is a modern 3D graphics engine built with C++, OpenGL, and GLSL. It demonstrates a complete real-time rendering pipeline, advanced lighting, model loading, collision detection, a particle system, and interactive camera/player controls. The codebase is modular, extensible, and suitable for both learning and further development.

---

## Table of Contents

- [Project Structure](#project-structure)
- [Core Concepts and Implementations](#core-concepts-and-implementations)
  - [1. Windowing and OpenGL Context](#1-windowing-and-opengl-context)
  - [2. Rendering Pipeline](#2-rendering-pipeline)
  - [3. Lighting System (Detailed)](#3-lighting-system-detailed)
    - [3.1 Directional Light](#31-directional-light)
    - [3.2 Point Light](#32-point-light)
    - [3.3 Spot Light](#33-spot-light)
  - [4. Object and Scene System](#4-object-and-scene-system)
    - [4.1 Model and Mesh System](#41-model-and-mesh-system)
    - [4.2 Collider System](#42-collider-system)
    - [4.3 Scene Composition and Setup](#43-scene-composition-and-setup)
  - [5. Shaders](#5-shaders)
  - [6. Texturing](#6-texturing)
  - [7. Camera and Player Controls](#7-camera-and-player-controls)
  - [8. Particle System](#8-particle-system)
  - [9. Utility and Helper Systems](#9-utility-and-helper-systems)
- [Assets and Resources](#assets-and-resources)
- [Build and Run](#build-and-run)
- [Extending the Engine](#extending-the-engine)

---

## Project Structure

```
GLSL/
  assets/         # Textures, models, cubemaps
  import/         # Third-party libraries (GLAD, GLFW, GLM, stb)
  shader/         # GLSL shaders (vertex, fragment)
  src/            # Engine source code (C++)
  main.cpp        # Main application entry point
  CMakeLists.txt  # Build configuration
  README.md       # This documentation
```

---

## Core Concepts and Implementations

### 1. Windowing and OpenGL Context

- **GLFW** is used for window creation, input handling, and OpenGL context management.
- **GLAD** is used for loading OpenGL function pointers.
- The window is initialized with a 3.3 core profile context, and the viewport is set to 1200x1200 pixels.
- The main loop runs until the window is closed, handling buffer swaps and event polling.

### 2. Rendering Pipeline

- **Depth Testing** is enabled to ensure correct 3D rendering order.
- The main loop:
  - Clears the color and depth buffers.
  - Updates the player and camera.
  - Animates lights and updates their properties.
  - Draws all scene objects (terrain, trees, farmhouse, lamp, etc.).
  - Renders light sources as visible meshes.
  - Draws the particle system (e.g., campfire smoke).
- **Frame Timing**: Delta time is calculated each frame for smooth, frame-rate-independent movement and animation.

---

### 3. Lighting System (Detailed)

Lighting is a core feature of the engine, supporting multiple light types, each with unique properties, behaviors, and shader logic. All lights are managed in a vector and sent to the shader as an array of structs. The fragment shader loops over all active lights and applies their effects per-pixel.

#### 3.1 Directional Light

- **Concept**: Simulates sunlight or other infinitely distant sources. All rays are parallel, and the light has a direction but no position.
- **Properties**:
  - `type = 0` (in code and shader)
  - `direction`: The direction the light is shining (vec3)
  - `color`: RGBA color and intensity (vec4)
- **Implementation**:
  - Created in C++ as `Light(0, glm::vec3(-1.0f), glm::vec3(-0.0f, -1.0f, -0.0f), glm::vec4(1.0f))`
  - Animated in the main loop by modulating its color intensity with a sine function for day/night simulation:
    ```cpp
    lights[0].color = glm::vec4(1.0f) * (0.3f + 0.7f * abs(sin(time * 0.5f)));
    ```
- **Shader Logic**:
  - In `default.frag`, if `light.type == 0`, the light direction is used for diffuse and specular calculations.
  - No attenuation is applied (infinite source).
  - Intensity can be adjusted in the shader for ambient/diffuse/specular.
- **Usage in Scene**:
  - Provides global illumination, simulating sunlight.
  - Affects all objects equally, regardless of their position.

#### 3.2 Point Light

- **Concept**: Emits light in all directions from a single point, like a bulb or torch.
- **Properties**:
  - `type = 1`
  - `position`: The world-space position of the light (vec3)
  - `color`: RGBA color and intensity (vec4)
  - Attenuation parameters (hardcoded in shader): `constant`, `linear`, `quadratic`
- **Implementation**:
  - Created in C++ as `Light(1, glm::vec3(-12.0f, 12.0f, 7.0f), glm::vec3(0.0f), glm::vec4(1.0f, 0.5f, 0.0f, 10.0f))`
  - Animated in the main loop for effects like flickering:
    ```cpp
    lights[1].color = glm::vec4(2.0f, 1.0f, 0.0f, 1.0f);
    ```
- **Shader Logic**:
  - In `default.frag`, if `light.type == 1`, the light direction is calculated as the vector from the fragment to the light position.
  - Attenuation is computed based on distance:
    ```glsl
    float dist = length(light.position - crntPos);
    float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));
    ```
  - Affects only objects within its effective radius.
- **Usage in Scene**:
  - Used for local light sources, e.g., lamps, fires.
  - Visualized as a small mesh (cube) at the light's position.

#### 3.3 Spot Light

- **Concept**: Emits a cone of light, like a flashlight or car headlamp. Has a position, direction, and cutoff angles.
- **Properties**:
  - `type = 2`
  - `position`: The world-space position (vec3)
  - `direction`: The direction the cone is pointing (vec3)
  - `color`: RGBA color and intensity (vec4)
  - Inner and outer cutoff angles (hardcoded in shader)
- **Implementation**:
  - Created in C++ as `Light(2, glm::vec3(20.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec4(0.0f, 0.5f, 1.0f, 2.0f))`
  - Animated in the main loop for effects like pulsing:
    ```cpp
    lights[2].color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) * (0.5f + 0.5f * cos(time));
    ```
- **Shader Logic**:
  - In `default.frag`, if `light.type == 2`, the light direction and cutoff angles are used to compute the spotlight effect:
    ```glsl
    float theta = dot(lightDir, normalize(-light.direction));
    float outerCutOff = 0.7;
    float innerCutOff = 0.85;
    float epsilon = innerCutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    diff *= intensity;
    spec *= intensity;
    attenuation *= intensity;
    ```
  - Attenuation and intensity are modulated by the angle between the fragment and the spotlight direction.
- **Usage in Scene**:
  - Used for focused light sources, e.g., spotlights, searchlights.
  - Visualized as a mesh at the light's position.

---

### 4. Object and Scene System

All object-related features are grouped here, including model and mesh management, collider setup, and scene composition.

#### 4.1 Model and Mesh System

- **Mesh**:
  - Represents a single drawable object, storing vertices, indices, textures, and a VAO.
  - Handles VBO/EBO setup and attribute linking.
  - Draws itself with a given shader and camera.
  - Provides min/max vertex queries for collider generation.
- **Model**:
  - Loads and manages complex 3D models (OBJ/MTL).
  - Supports multiple meshes and materials per model.
  - Loads geometry, normals, UVs, and material properties.
  - Handles texture assignment and tiling.
  - Can build a single collider for the whole model or per-material/component colliders.
  - Exposes methods for adding textures, setting tiling, and drawing with transformation matrices.
- **Texture Assignment**:
  - Models and meshes can have their textures replaced or tiled as needed.
  - Texture tiling is controlled via a uniform and per-model property.

#### 4.2 Collider System

- **Collider Types**:
  - **Box**: Axis-aligned bounding box, used for most objects and the player.
  - **Cylinder**: Used for tree trunks and lamp posts, with custom intersection logic.
- **Collider Generation**:
  - Each model can generate a collider (box or per-component).
  - Trees use a utility to generate a cylinder collider for each trunk, based on a reference trunk collider and their positions.
  - The player has a box collider (capsule-like) for first-person collision.
- **Collision Detection**:
  - Collision checks are performed before moving the player, axis by axis, to allow sliding along surfaces.
  - Specialized intersection logic for box-box, box-cylinder, and cylinder-cylinder (approximate).
  - All static scene objects (trees, farmhouse, lamp) have colliders added to a global list for collision checks.

#### 4.3 Scene Composition and Setup

- **Terrain**: Large plane mesh with grass texture, tiled for realism.
- **Trees**: Multiple tree models placed at various positions and scales, each with its own collider.
- **Farmhouse**: Imported model, rotated and scaled, with a box collider.
- **Lamp Post**: Imported model, rotated and scaled, with a cylinder collider.
- **Lighting**: Three lights (directional, point, spot) with animated properties.
- **Campfire**: Simulated with a particle system for smoke.
- **Draw Order**: Opaque objects are drawn first, then transparent particles.
- **Scene Setup in Code**:
  - All objects are loaded, transformed, and their colliders are created in `main.cpp`.
  - The scene is rendered each frame, with all objects, lights, and effects drawn in the correct order.

---

### 5. Shaders

- **Shader Management**: Shaders are loaded, compiled, and linked via a `Shader` class.
- **default.vert**: Vertex shader that transforms vertices, passes normals, colors, and texture coordinates.
- **default.frag**: Fragment shader implementing a Phong lighting model with support for multiple lights (directional, point, spot), texture tiling, and material properties.
- **light.vert/light.frag**: Minimal shaders for rendering light source meshes.
- **particle.vert/particle.frag**: Shaders for the particle system, supporting alpha blending and soft edges.
- **Uniforms**: Camera matrices, model matrices, light arrays, and material properties are passed as uniforms.

---

### 6. Texturing

- **Texture Class**: Loads images (using stb_image), creates OpenGL textures, and manages binding.
- **Texture Types**: Supports diffuse and specular maps.
- **Texture Tiling**: Uniform scaling of texture coordinates for repeating patterns (e.g., grass terrain).
- **Texture Assignment**: Models and meshes can have their textures replaced or tiled as needed.

---

### 7. Camera and Player Controls

- **Camera**:
  - Implements a first-person perspective.
  - Supports movement (WASD), mouse look, and adjustable speed.
  - Maintains view and projection matrices.
- **Player**:
  - Encapsulates the camera and a collider.
  - Handles gravity, jumping, and ground detection.
  - Processes input for movement and jumping.
  - Updates position based on collision checks and physics.
  - Prevents movement through objects using collision detection.

---

### 8. Particle System

- **ParticleSystem Class**:
  - Manages a list of particles, each with position, velocity, life, size, and alpha.
  - Emits new particles at a given origin (e.g., campfire).
  - Updates particle positions, fades them out, and removes dead particles.
  - Renders particles as textured quads with alpha blending.
- **Shaders**: Particle shaders support soft edges and transparency.
- **Usage**: In the main loop, several smoke particles are emitted per frame at the campfire position, creating a continuous smoke effect.

---

### 9. Utility and Helper Systems

- **tree_collider_utils.h**: Utility to generate colliders for all trees based on a reference trunk collider and their positions.
- **GLM**: Used for all vector/matrix math (positions, transformations, directions).
- **stb_image**: Used for loading image files as textures.
- **CMake**: Build system configuration for cross-platform compilation.

---

## Assets and Resources

- **assets/textures/**: Contains all texture images (grass, brick, smoke, etc.).
- **assets/objects/**: Contains 3D models (OBJ format) for terrain, trees, farmhouse, lamp.
- **assets/cubemaps/**: Contains skybox/environment textures (not always used in the main loop, but supported).

---

## Build and Run

1. **Dependencies**: Requires CMake, OpenGL, GLFW, GLM, and a C++17 compiler.
2. **Build**:
   ```sh
   mkdir build
   cd build
   cmake ..
   make
   ```
3. **Run**:
   ```sh
   ./3D_game
   ```
4. **Controls**:
   - `WASD`: Move player
   - `Mouse`: Look around
   - `Space`: Jump
   - `Shift`: Sprint

---

## Extending the Engine

- **Add new models**: Place OBJ files in `assets/objects/` and load them in `main.cpp`.
- **Add new textures**: Place images in `assets/textures/` and assign to models/meshes.
- **Add new lights**: Push new `Light` objects to the `lights` vector and update the shader.
- **Add new particle effects**: Instantiate new `ParticleSystem` objects with different textures and emission logic.
- **Implement new shaders**: Add GLSL files to `shader/` and load them via the `Shader` class.

---

## Conceptual Summary

This engine demonstrates the following advanced graphics concepts:

- **OpenGL context and window management**
- **Modern shader-based rendering pipeline**
- **OBJ/MTL model loading and material parsing**
- **Multiple mesh/material support per model**
- **Texture mapping and tiling**
- **Phong lighting with multiple light types**
- **Real-time light animation**
- **First-person camera and player physics**
- **Robust collision detection (box and cylinder)**
- **Particle systems for effects (smoke, fire, etc.)**
- **Efficient scene management and draw order**
- **Extensible, modular C++ architecture**

---

**This README is designed to be a complete technical reference for the project, suitable for use as the basis of a detailed report.** If you need further breakdowns (e.g., per-class, per-shader, or per-feature), let me know!
