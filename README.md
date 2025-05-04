# GLSL 3D Graphics Engine

This repository contains a comprehensive 3D graphics engine built with OpenGL and GLSL (OpenGL Shading Language). The project implements a modern rendering pipeline with advanced lighting, texturing, camera movement, collision detection, and model loading capabilities.

## Project Structure in Detail

### Core Files

- **main.cpp**: The entry point of the application.
  - Initializes GLFW and OpenGL context
  - Creates the rendering window
  - Sets up the player, lights, and scene objects
  - Contains the main rendering loop
  - Manages input handling via GLFW callbacks

- **CMakeLists.txt**: The CMake configuration that:
  - Defines the project and its version
  - Locates necessary dependencies (OpenGL, GLFW, GLM)
  - Sets up compilation flags and options
  - Configures the build process
  - Sets up executable targets and links libraries

- **default.frag**: The main fragment shader that:
  - Implements the lighting system (point, directional, spot lights)
  - Calculates material properties with ambient, diffuse, and specular components
  - Handles texture sampling and tiling
  - Calculates lighting attenuation
  - Applies post-processing effects

- **test.cpp**: A demo application that:
  - Sets up a 3D scene with various models (terrain, trees, farmhouse)
  - Demonstrates lighting with directional and spot lights
  - Implements collision detection between player and scene objects
  - Shows how to use textures and materials
  - Features a player-controlled camera with WASD movement

### Source Directory (`src/`)

#### Rendering Primitives

- **VBO (Vertex Buffer Object)**:
  - `VBO.h`, `VBO.cpp`: Manages GPU memory for vertex data
  - Handles creation, binding, data loading, and deletion of vertex buffers
  - Provides methods for sending vertex attributes to the GPU

- **VAO (Vertex Array Object)**:
  - `VAO.h`, `VAO.cpp`: Manages vertex attribute configurations
  - Links attributes to VBOs
  - Configures the vertex layout (positions, normals, UVs, etc.)
  - Provides methods for binding and unbinding VAOs during rendering

- **EBO (Element Buffer Object)**:
  - `EBO.h`, `EBO.cpp`: Manages index data for indexed rendering
  - Optimizes rendering by allowing vertex reuse
  - Provides methods for binding, loading data, and deletion

#### Shader Management

- **shaderClass**:
  - `shaderClass.h`, `shaderClass.cpp`: Core shader functionality
  - Loads and compiles vertex, fragment, and geometry shaders
  - Handles shader linking and program creation
  - Provides uniform setting methods for various data types
  - Error checking and shader validation

#### Texturing System

- **Texture**:
  - `Texture.h`, `Texture.cpp`: Handles texture loading and management
  - Supports different texture types (diffuse, specular, normal maps)
  - Configures texture parameters (filtering, wrapping)
  - Uses stb_image for image file loading
  - Supports various pixel formats (RGB, RGBA, etc.)

#### Model Loading

- **Mesh**:
  - `Mesh.h`, `Mesh.cpp`: Represents a single mesh within a 3D model
  - Stores vertices, indices, and textures
  - Contains methods for setup and drawing
  - Manages VAOs, VBOs, and EBOs for each mesh

- **Model**:
  - `model.h`, `model.cpp`: Loads and manages complex 3D models
  - Uses Assimp library to import various 3D file formats
  - Processes model data (vertices, normals, textures)
  - Handles material properties from model files
  - Organizes hierarchical mesh structures

#### Camera System

- **Camera**:
  - `Camera.h`, `Camera.cpp`: Implements a 3D camera system
  - Manages view and projection matrices
  - Provides movement functions (forward, backward, strafe)
  - Handles mouse input for camera rotation
  - Implements perspective projection and field of view adjustments

#### Player and Collision

- **Player**:
  - `Player.h`, `Player.cpp`: Represents the user/player entity
  - Contains the camera and manages player position
  - Handles user input for movement
  - Manages collision detection with the environment
  - Updates player state based on delta time

- **Collider**:
  - `Collider.h`: Implements collision detection
  - Defines collision shapes (boxes, spheres)
  - Provides methods for collision testing
  - Used to prevent camera/player from moving through objects

#### Lighting System

- **Light**:
  - `Light.h`, `Light.cpp`: Manages different light types
  - Implements point lights (omni-directional)
  - Implements directional lights (sun-like)
  - Implements spotlights (cone-shaped)
  - Stores light properties (position, direction, color, intensity)

#### Skybox/Environment

- **Cubemaps**:
  - `Cubemaps.h`, `Cubemaps.cpp`: Implements skybox functionality
  - Loads and renders environment cube maps
  - Provides methods for binding and rendering the skybox
  - Handles special texture sampling for environment reflections

### Shader Directory (`shader/`)

- **default.vert**: The main vertex shader that:
  - Transforms vertices using model-view-projection matrices
  - Calculates and outputs vertex normals
  - Passes texture coordinates to the fragment shader
  - Handles vertex attributes (position, normal, color, UV)

- **default.frag**: The main fragment shader (described above)

- **light.vert**, **light.frag**: Simplified shaders for rendering light sources
  - Minimal processing for light objects
  - Simple transformation and basic coloring

- **skybox.vert**, **skybox.frag**: Specialized shaders for rendering the skybox
  - Handles cubemap texture sampling
  - Ensures the skybox is always centered around the camera

### Assets Directory (`assets/`)

- **textures/**: Contains texture files:
  - Diffuse textures (color/albedo maps) like "herbe.png"
  - Specular maps for controlling shininess
  - Normal maps for surface detail

- **objects/**: Contains 3D model files:
  - Various OBJ format models with materials
  - Includes primitives and complex models

- **cubemaps/**: Environment map textures for skyboxes
  - Six-sided cube maps for environment rendering

## Detailed Technical Implementation

### Rendering Pipeline

1. **Initialization**:
   - GLFW window creation
   - OpenGL context setup
   - Shader compilation and linking
   - Texture loading
   - Model loading

2. **Main Loop**:
   - Clear buffers
   - Update player position and camera
   - Process input
   - Update scene state (lights, animations)
   - Render objects with appropriate shaders
   - Apply post-processing effects
   - Swap buffers and poll events

3. **Shutdown**:
   - Clean up OpenGL resources
   - Terminate GLFW
   - Free allocated memory

### Lighting System in Detail

The project implements a Phong lighting model with three components:

1. **Ambient Lighting**: 
   - Basic illumination present in all parts of the scene
   - Controlled by material ambient properties
   - Provides minimum visibility even in shadows

2. **Diffuse Lighting**:
   - Directional illumination based on light-surface angle
   - Calculated using the dot product of surface normal and light direction
   - Creates the primary shading effect on surfaces

3. **Specular Lighting**:
   - Creates highlights on reflective surfaces
   - Calculated using the reflection vector and view direction
   - Controlled by material shininess property
   - Enhances the appearance of glossy/metallic surfaces

The engine supports three light types:

1. **Point Light** (`pointLight()` in default.frag):
   - Emits light in all directions
   - Features distance attenuation (quadratic and linear)
   - Simulates light bulbs or small light sources

2. **Directional Light** (`direcLight()` in default.frag):
   - Light from a consistent direction (like sunlight)
   - No attenuation with distance
   - Uniform illumination across the scene

3. **Spotlight** (`spotLight()` in default.frag):
   - Cone-shaped light with inner and outer boundaries
   - Controlled angle and falloff
   - Simulates flashlights, headlamps, etc.

### Material System

Materials are represented by a struct in the shaders:
```glsl
struct Material {
    vec3 ambient;   // Ka: Ambient reflection color
    vec3 diffuse;   // Kd: Diffuse reflection color
    vec3 specular;  // Ks: Specular reflection color
    float shininess; // Ns: Specular exponent
};
```

These properties control how surfaces interact with light:
- Higher ambient values make objects visible even in shadow
- Diffuse controls the main surface color under direct light
- Specular determines the color and intensity of highlights
- Shininess controls the size of specular highlights (higher = smaller, sharper)

### Texture System

The engine supports multiple texture types:
1. **Diffuse Maps**: Control surface color
2. **Specular Maps**: Control shininess distribution

Textures are loaded using stb_image and bound to appropriate texture units. The fragment shader samples these textures and applies tiling based on the `textureTiling` uniform.

### Camera System

The camera uses a standard first-person setup with:
- Position vector (location in 3D space)
- Front vector (direction the camera is facing)
- Up vector (orientation)
- Right vector (for strafing)

Camera movement is controlled via keyboard inputs:
- W/A/S/D for forward/left/backward/right movement
- Space for upward movement
- Left Control for downward movement

Camera rotation is controlled via mouse movement:
- Mouse X controls yaw (horizontal rotation)
- Mouse Y controls pitch (vertical rotation)
- Includes constraints to prevent gimbal lock

### Collision System

The `Collider` class implements basic collision detection to prevent the player from moving through objects:
- Defines collision shapes (primarily boxes)
- Tests player position against world objects
- Adjusts player position when collisions occur

## Building and Running the Project

### Prerequisites

- OpenGL 3.3+ compatible graphics card and drivers
- CMake 3.10+
- C++11 compatible compiler
- GLFW3
- GLM (OpenGL Mathematics)

### Build Instructions

1. Clone the repository
2. Navigate to the project directory
3. Run CMake:
   ```
   cmake .
   ```
4. Build the project:
   ```
   make
   ```
5. Run the executable:
   ```
   ./bin/3D_Game_main
   ```
   
   Or run the test application:
   ```
   ./bin/3D_Game_test
   ```

### Controls

- **W, A, S, D**: Move forward, left, backward, right
- **Space**: Move up
- **Left Control**: Move down
- **Mouse**: Look around
- **Escape**: Close the application

## Running the Test Application (test.cpp)

The repository includes a demo application (`test.cpp`) that demonstrates the core features of the engine. The test application:

1. **Creates a 3D scene** with:
   - A large flat terrain
   - A tree model with textures
   - A farmhouse model with textures
   - Light sources (directional and spotlight)

2. **Implements lighting system** with:
   - A directional light representing the sun
   - A spotlight at a high position pointing downward
   - Dynamic lighting control with adjustable intensity

3. **Sets up collision detection**:
   - The tree and farmhouse have collision boxes
   - The player cannot pass through these objects
   - Collision events are logged to the console

4. **Demonstrates texture mapping**:
   - Loads and applies textures to terrain and models
   - Uses material properties from model files
   - Shows different texture types (diffuse maps)

### Running the Test Application

To build and run the test application:

```bash
# Build the project
cmake .
make

# Run the test application
./bin/3D_Game_test
```

### Test Application Structure

The test application follows these steps:

1. **Initialization**:
   ```cpp
   // Initialize GLFW and create window
   glfwInit();
   GLFWwindow* window = glfwCreateWindow(width, height, "3D_game", NULL, NULL);
   
   // Load GLAD for OpenGL function access
   gladLoadGL();
   ```

2. **Asset Loading**:
```cpp
   // Load textures
   Texture textures[] { Texture((texPath + "herbe.png").c_str(), "diffuse", 0) };
   
   // Load shaders
   Shader shaderProgram("shader/default.vert", "shader/default.frag");
   
   // Load 3D models
   Model terrainModel("assets/objects/plane.obj");
   Model treeModel("assets/objects/Tree 02/Tree.obj");
   Model farmhouseModel("assets/textures/newhouse/farmhouse_obj.obj");
   ```

3. **Scene Setup**:
   ```cpp
   // Set up model transformations
   glm::mat4 terrainModelMatrix = glm::mat4(1.0f);
   terrainModelMatrix = glm::scale(terrainModelMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
   
   // Create colliders for scene objects
   treeModel.buildCollider(treeModelMatrix);
   farmhouseModel.buildCollider(farmhouseModelMatrix);
   
   // Setup lighting
   std::vector<Light> sceneLights;
   sceneLights.emplace_back(0, glm::vec3(0.0f), glm::vec3(-0.2f, -1.0f, -0.3f), 
                          glm::vec4(1.0f, 1.0f, 0.9f, 1.0f), nullptr);
   ```

4. **Main Loop**:
   ```cpp
   while (!glfwWindowShouldClose(window))
   {
       // Clear buffers
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       
       // Update player
       player.Update(window, worldColliders, deltaTime);
       
       // Update lighting
       sceneLights[0].color = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f) * sunStrength;
       
       // Draw models
       treeModel.Draw(shaderProgram, player.camera, treeModelMatrix);
       farmhouseModel.Draw(shaderProgram, player.camera, farmhouseModelMatrix);
       terrainModel.Draw(shaderProgram, player.camera, terrainModelMatrix);
       
       // Swap buffers and handle events
       glfwSwapBuffers(window);
       glfwPollEvents();
   }
   ```

5. **Cleanup**:
   ```cpp
   // Delete resources
   shaderProgram.Delete();
   lightShader.Delete();
   glfwDestroyWindow(window);
   glfwTerminate();
   ```

## Modifying the Project

### Adding New Models

1. Add your OBJ file to the `assets/objects/` directory
2. Load the model in `main.cpp`:
   ```cpp
   Model yourModel("assets/objects/your_model.obj");
   ```
3. Render the model in the main loop:
   ```cpp
   yourModel.Draw(shaderProgram, camera);
   ```

### Adding New Textures

1. Add texture files to `assets/textures/`
2. Create texture objects in `main.cpp`:
   ```cpp
   Texture textures[]
   {
       Texture((texPath + "your_texture.png").c_str(), "diffuse", 0),
       Texture((texPath + "your_specular.png").c_str(), "specular", 1)
   };
   ```

### Customizing Lighting

1. Modify light parameters in `main.cpp`:
```cpp
   // Create a point light
   sceneLights.emplace_back(
       1, // type (0=directional, 1=point, 2=spot)
       glm::vec3(x, y, z), // position
       glm::vec3(0.0f), // direction (not used for point lights)
       glm::vec4(r, g, b, a), // color and intensity
       nullptr // associated mesh
   );
   ```

2. Adjust lighting calculations in `default.frag`:
   - Change attenuation parameters for point lights
   - Modify cone angles for spotlights
   - Adjust ambient, diffuse, or specular factors

### Creating Custom Shaders

1. Create new .vert and .frag files in the shader directory
2. Load them in `main.cpp`:
```cpp
   Shader customShader("shader/custom.vert", "shader/custom.frag");
```
3. Update uniforms and use for rendering:
```cpp
   customShader.Activate();
   customShader.setVec3("camPos", camera.Position);
   // Set other uniforms
   model.Draw(customShader);
   ```

## Advanced Features

### Depth-Based Effects

The `linearrizeDepth` function in the fragment shader converts non-linear depth values to linear ones, enabling:
- Fog effects
- Depth-based color adjustments
- Distance attenuation effects

### Material-Based Rendering

The material system allows for:
- Physically-based rendering approaches
- Material-specific shader behaviors
- Complex surface appearance simulation

### Multi-Light Scene Management

The `sceneLights` vector allows for dynamic management of multiple light sources:
- Add/remove lights during runtime
- Animate light positions and colors
- Create complex lighting scenarios

### Collision-Aware Camera

The player-collider integration prevents camera clipping through:
- Terrain
- Buildings
- Props and scene objects

## Performance Considerations

- **VBO/VAO/EBO Management**: Optimizes GPU memory usage
- **Shader Optimization**: Keeps shader complexity reasonable
- **Model Loading**: Efficiently processes 3D model data
- **Texture Management**: Properly handles texture units and bindings

## Future Improvements

- Shadow mapping for realistic shadows
- Deferred rendering for better performance with many lights
- PBR (Physically-Based Rendering) implementation
- Post-processing effects (bloom, ambient occlusion, etc.)
- Animation system for skeletal mesh animation
